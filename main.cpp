#include <iostream>
#include <sys/shm.h>
#include "ShareMemoryAnalyse.h"
#include "XmlLoader.h"

int main() {
    int shmid; //设置共享内存shmid

    std::cout << "input shmid: ";
    std::cin >> shmid;
    ShareMemoryAnalyse *analyse = new ShareMemoryAnalyse();
    analyse->loadXml();
    analyse->linkMem(shmid);
    while (true) {
        analyse->setAndLoadSql();
        analyse->analyseSqlStruct();
        analyse->setArgs();
        analyse->findData(true);
    }




//    analyse->setSearchData();
//    if (analyse->search(true)) {
//        analyse->printData();
//    } else {
//        std::cout << "no data";
//    }

    return 0;
}