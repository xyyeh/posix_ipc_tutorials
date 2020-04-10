#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "shm_sem.h"

using namespace std;
int main(int argc, char* argv[]) {
  try {
    ShmSemaphore shmMemory("/testSharedmemory1");

    shmMemory.Create(100);
    shmMemory.Attach();
    char* str = (char*)shmMemory.GetData();

    if (std::string(argv[1]) == "1") {
      std::cout << "1" << std::endl;
      for (int i = 0; i < 10; i++) {
        char sTemp[10];
        shmMemory.Lock();
        sprintf(sTemp, "Data:%d", rand() % 100);
        strcpy(str, sTemp);
        printf("\nWriting:%s", str);
        shmMemory.UnLock();
        sleep(4);
      }
    } else {
      std::cout << "2" << std::endl;
      for (int i = 0; i < 10; i++) {
        char sTemp[10];
        printf("\nReading:%d", i + 1);
        shmMemory.Lock();
        printf("--->%s", str);
        shmMemory.UnLock();
        sleep(4);
      }
    }

  } catch (std::exception& ex) {
    cout << "Exception:" << ex.what();
  }
}