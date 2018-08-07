//
// Created by sunshine on 18-7-20.
//

#include <sys/shm.h>
#include <iostream>
#include <cstring>
#include <string>
#include <queue>
#include <fstream>
#include <stack>
#include "MemManager.h"
#include "Struct.h"

bool MemManager::attachMem(const int &shmid) {
    this->shm = shmat(shmid, (void *) nullptr, 0);
    if (this->shm == (void *) -1) {
        std::cout << "attach memory error" << std::endl;
        return false;
    }
    //大小统计
    shmid_ds shmidDs;
    shmctl(shmid, IPC_STAT, &shmidDs);
    this->shmSize = shmidDs.shm_segsz;
    return true;
}

bool MemManager::leaveMem() {
    if (shmdt(this->shm) == -1) {
        std::cout << "shmdt failed." << std::endl;
        return false;
    }
    return true;
}

MemManager::MemManager() {
}

MemManager::~MemManager() {
    leaveMem();
}

bool MemManager::isEqual(void *lh, void *rh, const int &dataType) {
//todo update here
    switch (dataType) {
        case _INT:
            return *(int *) lh == *(int *) rh;
        case _CHAR:
            return *(char *) lh == *(char *) rh;
        case _STRING:
            return (strcmp((char *) lh, (char *) rh) == 0);
        case _UINT8_T:
            return *(uint8_t *) lh == *(uint8_t *) rh;
        case _UINT16_T:
            return *(uint16_t *) lh == *(uint16_t *) rh;
        case _UINT32_T:
            return *(uint32_t *) lh == *(uint32_t *) rh;
        case _INT8_T:
            return *(int8_t *) lh == *(int8_t *) rh;
        case _INT16_T:
            return *(int16_t *) lh == *(int16_t *) rh;
        case _INT32_T:
            return *(int32_t *) lh == *(int32_t *) rh;
        case _UNSIGNED_CHAR:
            return *(unsigned char *) lh == *(unsigned char *) rh;
        case _SHORT:
            return *(short *) lh == *(short *) rh;
        case _FLOAT:
            return *(float *) lh == *(float *) rh;
        case _DOUBLE:
            return *(double *) lh == *(double *) rh;
        default:
            std::cout << "not support type";
            break;
    }
}

void MemManager::nextStructBlock() {
    this->curShm = (void *) (((char *) this->curShm) + this->structSize);
}

void MemManager::setStructList(std::vector<StructNode *> &structList) {
    this->structList = structList;
}

bool MemManager::linkMem(const int &shmid) {
    return this->attachMem(shmid);
}

void MemManager::setQueryConditionList(std::vector<QueryCondition *> &queryConditionList) {
    this->queryConditionList = queryConditionList;
}

//todo 
/**
 * 目前只支持and语句，如果遇到数组的时候，应该将数组展开条件变为or进行查询
 * @param isFirst
 * @return
 */
bool MemManager::search(const bool &isFirst) {
    if (isFirst) {
        this->curShm = this->shm;
    }
    int a = 0;
    bool continueLoop;

    while (true) {
        continueLoop = false;
        if (this->isMemOverFlow()) {
            return false;
        }

        int curQueryId = 0;
        for (int j = 0; j < this->queryConditionTotalNum; ++j) {
            for (int i = 0; i < queryConditionList.size(); ++i) {
                continueLoop = false;
                //按照queryId分组查询
                if (queryConditionList[i]->queryId != curQueryId) {
                    continue;
                }
                //todo update here
                switch (queryConditionList[i]->basicType) {
                    case _INT:
                        this->iData = (int) std::stoi(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->iData, _INT);
                            break;
                        }
                    case _CHAR:
                        this->cData = (char) queryConditionList[i]->value[0];
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->cData, _CHAR);
                            break;
                        }
                    case _STRING:
                        this->strData = const_cast<char *>(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) this->strData, _STRING);
                            break;
                        }
                    case _UINT8_T:
                        this->uint8Data = (unsigned char) queryConditionList[i]->value[0];
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->uint8Data, _UINT8_T);
                            break;
                        }
                    case _UINT16_T:
                        this->uint16Data = (uint16_t) std::stoul(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->uint16Data, _UINT16_T);
                            break;
                        }
                    case _UINT32_T:
                        this->uint32Data = (uint32_t) std::stoul(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->uint32Data, _UINT32_T);
                            break;
                        }
                    case _INT8_T:
                        this->int8Data = (char) queryConditionList[i]->value[0];
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->int8Data, _INT8_T);
                            break;
                        }
                    case _INT16_T:
                        this->int16Data = (int16_t) std::stoi(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->int16Data, _INT16_T);
                            break;
                        }
                    case _INT32_T:
                        this->int32Data = (int32_t) std::stoi(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->int32Data, _INT32_T);
                            break;
                        }
                    case _UNSIGNED_CHAR:
                        this->ucData = (unsigned char) queryConditionList[i]->value[0];
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->ucData, _UNSIGNED_CHAR);
                            break;
                        }
                    case _SHORT:
                        this->siData = (short int) std::stoi(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->siData, _SHORT);
                            break;
                        }
                    case _FLOAT:
                        this->fData = (float) std::stof(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->fData, _FLOAT);
                            break;
                        }
                    case _DOUBLE:
                        this->dData = (double) std::stod(queryConditionList[i]->value);
                        {
                            continueLoop = isEqual((char *) this->curShm + queryConditionList[i]->index,
                                                   (void *) &this->dData, _DOUBLE);
                            break;
                        }
                }
                if (continueLoop) {
                    break; //只要一个query子条件满足条件则跳出
                } else {
                    continue; //继续寻找
                }
            }
            if (continueLoop) {
                curQueryId++;
                continue; //一个query父条件寻找到则继续
            } else {
                break; //都不满足进行短路操作
            }
        }
        if (continueLoop) { //退出时continueLoop是true,则寻找到
            return true;
        }
        this->nextStructBlock();
        a++;
    }
}

void MemManager::setStructSize(const int &size) {
    this->structSize = size;
}

bool MemManager::findData(const bool &isFirst) {
    if (this->search(isFirst)) {
        if (!this->filename.empty()) {
            this->printDataToFile();
        } else {
            this->printData();
        }
    } else {
        std::cout << "no data found." << std::endl;
        return false;
    }

    while (true) {
        //next
        this->nextStructBlock();
        if (this->isMemOverFlow()) {
            return false;
        }
        if (this->search(false)) {
            if (!this->filename.empty()) {
                this->printDataToFile();
            } else {
                this->printData();
            }
        }
    }
}

bool MemManager::printData() {
    for (int i = 0; i < this->printNodeList.size(); ++i) {
        //跳过打印block类型
        if (printNodeList[i].type == _BLOCK) {
            continue;
        }
        std::cout << printNodeList[i].name << ": ";
        printByType((void *) (printNodeList[i].index + (char *) this->curShm), printNodeList[i].type);
        std::cout << std::endl;
    }
    std::cout << "--------------------" << std::endl;
    return true;
}

bool MemManager::printByType(void *loc, const int &type) {
    //todo update here
    switch (type) {
        case _INT:
            std::cout << *(int *) loc;
            break;
        case _CHAR:
            std::cout << *(char *) loc;
            break;
        case _STRING:
            //todo need test, because over when '\0'
            std::cout << (char *) loc;
            break;
        case _UINT8_T:
            std::cout << *(uint8_t *) loc;
            break;
        case _UINT16_T:
            std::cout << *(uint16_t *) loc;
            break;
        case _UINT32_T:
            std::cout << *(uint32_t *) loc;
            break;
        case _INT8_T:
            std::cout << *(int8_t *) loc;
            break;
        case _INT16_T:
            std::cout << *(int16_t *) loc;
            break;
        case _INT32_T:
            std::cout << *(int32_t *) loc;
            break;
        case _UNSIGNED_CHAR:
            std::cout << *(unsigned char *) loc;
            break;
        case _SHORT:
            std::cout << *(short *) loc;
            break;
        case _FLOAT:
            std::cout << *(float *) loc;
            break;
        case _DOUBLE:
            std::cout << *(double *) loc;
            break;
        default:
            std::cout << "not support type";
            break;

    }
    return true;
}

bool MemManager::printByType(void *loc, const int &type, std::ofstream &outFile) {
    //todo update here
    switch (type) {
        case _INT:
            outFile << *(int *) loc;
            break;
        case _CHAR:
            outFile << *(char *) loc;
            break;
        case _STRING:
            //todo need test, because over when '\0'
            outFile << (char *) loc;
            break;
        case _UINT8_T:
            outFile << *(uint8_t *) loc;
            break;
        case _UINT16_T:
            outFile << *(uint16_t *) loc;
            break;
        case _UINT32_T:
            outFile << *(uint32_t *) loc;
            break;
        case _INT8_T:
            outFile << *(int8_t *) loc;
            break;
        case _INT16_T:
            outFile << *(int16_t *) loc;
            break;
        case _INT32_T:
            outFile << *(int32_t *) loc;
            break;
        case _UNSIGNED_CHAR:
            outFile << *(unsigned char *) loc;
            break;
        case _SHORT:
            outFile << *(short *) loc;
            break;
        case _FLOAT:
            outFile << *(float *) loc;
            break;
        case _DOUBLE:
            outFile << *(double *) loc;
            break;
        default:
            outFile << "not support type";
            break;

    }
    return true;
}


void MemManager::setShmOffset(int &offset) {
    this->shm = (void *) ((char *) this->shm + offset);
}

void MemManager::setOutputFilename(const std::string &filename) {
    this->filename = filename;
}

bool MemManager::printDataToFile() {
    std::ofstream outFile;
    outFile.open(this->filename, std::ios::out | std::ios::app);
    outFile.seekp(outFile.end);
    for (int i = 0; i < this->printNodeList.size(); ++i) {
        //跳过打印block类型
        if (printNodeList[i].type == _BLOCK) {
            continue;
        }
        outFile << printNodeList[i].name << ": ";
        printByType((void *) (printNodeList[i].index + (char *) this->curShm), printNodeList[i].type, outFile);
        outFile << std::endl;
    }
    outFile << "--------------------" << std::endl;
    outFile.close();
    return true;
}

bool MemManager::isMemOverFlow() {
    if ((size_t) this->curShm - (size_t) shm + this->structSize > this->shmSize) {
        return true;
    }
    return false;
}

void MemManager::setQueryConditionTotalNum(const int &count) {
    this->queryConditionTotalNum = count;
}

void MemManager::setPrintNode() {
    StructNode *structNode;
    std::queue<StructNode *> structNodeStack;
    structNodeStack.push(structList[0]);
    while (!structNodeStack.empty()) {
        structNode = structNodeStack.front();
        structNodeStack.pop();
        if (structNode->isLeaf) {
            this->printNodeList.push_back({structNode->name, structNode->structTypeInfo->type, structNode->index});
        }
        for (int i = 0; i < structNode->structNodeList.size(); ++i) {
            structNodeStack.push(structNode->structNodeList[i]);
        }
    }
}
