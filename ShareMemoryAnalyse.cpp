//
// Created by sunshine on 18-7-16.
//

#include "ShareMemoryAnalyse.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <iostream>
#include <sys/user.h>
#include "config/Config.h"


ShareMemoryAnalyse::ShareMemoryAnalyse() {
    this->xmlLoader = new XmlLoader;
    this->memManager = new MemManager;
    this->sqlAnalyse = new SqlAnalyse;
    this->offset = 0;
};

void ShareMemoryAnalyse::reportError(const int &level, const std::string &str) {
    if (level > LEVEL) {
        std::cout << str << std::endl;
    }
}

void ShareMemoryAnalyse::getFirstInfo(const std::string &str) {
//    structBlock* shared = (structBlock *)(this->shm);

}

bool ShareMemoryAnalyse::loadXml() {
    std::cout << "start load xml config for data structure." << std::endl;
    return this->xmlLoader->load();
}

ShareMemoryAnalyse::~ShareMemoryAnalyse() {

}

bool ShareMemoryAnalyse::linkMem(const int &shmid) {
    int offset;
    if(memManager->linkMem(shmid)) {
        std::cout << "input mem offset(查询的数据段的首条数据在内存中的起始位置): ";
        std::cin >> offset;
        memManager->setShmOffset(offset);
    }

    return 0;
}

bool ShareMemoryAnalyse::setAndLoadSql() {
    this->sqlAnalyse->setAndLoadSql();
    return true;
}

bool ShareMemoryAnalyse::analyseSqlStruct() {
    for (int i = 0; i < this->sqlAnalyse->queryConditionList.size(); ++i) {
        this->sqlAnalyse->queryConditionList[i]->basicType = this->xmlLoader->getBasicType(this->sqlAnalyse->queryConditionList[i]->columnNameList);
        this->sqlAnalyse->queryConditionList[i]->index = this->xmlLoader->getDataIndex(this->sqlAnalyse->queryConditionList[i]->columnNameList);
    }
    return true;
}

void ShareMemoryAnalyse::setArgs() {
    memManager->setStructList(xmlLoader->structList);
    memManager->setQueryConditionList(sqlAnalyse->queryConditionList);
    memManager->setStructSize(xmlLoader->structList[0]->structTypeInfo->size);
    memManager->setOutputFilename(sqlAnalyse->filename);
}

bool ShareMemoryAnalyse::findData(const bool &isFirst) {
    return memManager->findData(isFirst);
}


