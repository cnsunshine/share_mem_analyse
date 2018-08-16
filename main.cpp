#include <iostream>
#include <sys/shm.h>
#include "ShareMemoryAnalyse.h"
#include "XmlLoader.h"

int main() {
    int shmid; //设置共享内存shmid
    key_t key;

    std::cout << "input share key: ";
    std::cin >> key;
    shmid = shmget(key, 0, 0);
//    std::cout << "input shmid: ";
//    std::cin >> shmid;
    ShareMemoryAnalyse *analyse = new ShareMemoryAnalyse();
    analyse->loadXml();
    analyse->linkMem(shmid);
    while (true) {
        analyse->setAndLoadSql();
        analyse->setArgs();
        analyse->findData(true);
        analyse->recycleMemory();
    }
}