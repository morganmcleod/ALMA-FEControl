#include "../DLL/lv_wrapper.h"
#include <stdio.h>
#include <vector>

int main(int, char*[]) {

    std::vector<float> vec = {0.1,0.2};
    short result=TestSocketClient();
    printf("result=%d\n", result);
    return 0;   
}
