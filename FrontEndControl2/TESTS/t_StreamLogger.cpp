#include <logger.h>
#include <pthread.h>

const int threadCount=1000;
const int messageCount=1000;

void *thread1(void *arg) {
    int myNum = *((int *) arg);
    
    for (int msg = 0; msg < messageCount; ++msg) {
        LOG(LM_INFO) << "Thread " << myNum << " message " << msg
                     << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg << "," << msg
                    << std::endl;
    }        
    pthread_exit(NULL);
    return NULL;
}

int main(int, char*[]) {

    LOG(LM_INFO) << "test 1 2" << std::endl;
    
    pthread_t threads[threadCount];
    int data[threadCount];
    for (int thread = 0; thread < threadCount; ++thread) {
        data[thread] = thread;
        pthread_create(&threads[thread], NULL, thread1, &data[thread]);    
        LOG(LM_INFO) << "Main Thread created " << thread << std::endl;
    }
    
    void *tr;
    for (int thread = 0; thread < threadCount; ++thread) {
        pthread_join(threads[thread], &tr);
    }

    return 0;   
}
