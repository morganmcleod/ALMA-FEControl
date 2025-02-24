#include "portable.h"
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

struct listElem {
    int num;
    sem_t *synchLock;
    
    listElem(int _num = 0, sem_t *_synchLock = NULL)
      : num(_num),
        synchLock(_synchLock)
        {}

    ~listElem()
      {}
};

const bool JOIN_THREADS = false;
const int COUNT = 1000;

listElem list1[COUNT];
listElem list2[COUNT];
int pos1, pos2, end1, end2;

// mutexes to protect the lists: 
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

// flags to tell the threads to stop:
bool shutdownNow;
bool shutdownDone1;
bool shutdownDone2;

// thread 1 processes list1:
void *thread1(void *arg) {
    listElem E;
    while (true) {
        if (shutdownNow) {
            shutdownDone1 = true;
            pthread_exit(NULL);
        }       
        pthread_mutex_lock(&mutex1);
        if (end1 == pos1)
            pthread_mutex_unlock(&mutex1);
        
        else {        
            // get the next element from the list:
            E = list1[pos1++];
            pthread_mutex_unlock(&mutex1);
            
            // save the original semaphore:
            sem_t *sem1 = E.synchLock;
            
            // create and initialize a new semaphore.
            // substitute it for the original:
            sem_t sem2;
            E.synchLock = &sem2;
            sem_init(E.synchLock, 0, 0);
            
            // put the item in list2 for processing by thread2:
            pthread_mutex_lock(&mutex2);
            list2[end2++] = E;
            pthread_mutex_unlock(&mutex2);
            
            // Wait on, then destroy the substitute semaphore:
            sem_wait(E.synchLock);
            sem_destroy(E.synchLock);
            
            // put back and post on the original semaphore:
            E.synchLock = sem1;
            sem_post(E.synchLock);

            printf("thread1: %d done\n", E.num);
        }
        Sleep(10);
    }
}

// thread2 processes list2:
void *thread2(void *arg) {
    listElem E;
    while (true) {
        if (shutdownNow) {
            shutdownDone2 = true;
            pthread_exit(NULL);
        }       
        pthread_mutex_lock(&mutex2);
        if (end2 == pos2) 
            pthread_mutex_unlock(&mutex2);
        
        else {
            E = list2[pos2++];
            pthread_mutex_unlock(&mutex2);
            
            sem_post(E.synchLock);

            printf("thread2: %d done\n", E.num);
        }
        Sleep(10);
    }
}

int main(int, char*[]) {
    // Initialize flags and indexes:
    shutdownNow = shutdownDone1 = shutdownDone2 = false;
    pos1 = pos2 = end1 = end2 = 0;
    
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // Pause to look at Task Manager.  Handles = 10:
    Sleep(5000);
    
    sem_t synchLocks[COUNT];
    for (int index = 0; index < COUNT; ++index) {
        sem_init(&synchLocks[index], 0, 0);
        listElem E(index, &synchLocks[index]);
        list1[end1++] = E;
    }

    // Handles is between 2015 and 2019.  
    // With pthreads_win32 ver. 2.7.0 it would start to leak handles... 

    pthread_t T1;    
    pthread_create(&T1, NULL, thread1, NULL);

    pthread_t T2;
    pthread_create(&T2, NULL, thread2, NULL);

    if (!JOIN_THREADS) {
        pthread_detach(T1);
        pthread_detach(T2);
    }
    
    while (end1 > pos1 || end2 > pos2)
        Sleep(10);
    
    // Pause to look at Task Manager:  
    // With 2.7.0 Handles = 2151 with joinable threads (varies) 
    //            Handles = 2265 with detached threads (varies)
    // With 2.8.0 Handles holds between 2015 and 2019.
    Sleep(5000);
    
    shutdownNow = true;
    void *tr;
    if (JOIN_THREADS) {
        pthread_join(T1, &tr);
        pthread_join(T2, &tr);
    }
    
    while (!shutdownDone1 && !shutdownDone2)
        Sleep(10);

    for (int index = 0; index < COUNT; ++index)
        sem_destroy(&synchLocks[index]);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    // Pause to look at Task Manager: 
    // With 2.7.0 Handles = 148 with joinable threads (varies)
    //            Handles = 268 with detached threads (varies)
    // With 2.8.0 Handles = 9 or 12.
    Sleep(5000);
    
    printf("done\n");    
    return 0;
}


