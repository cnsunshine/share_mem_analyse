//
// Created by sunshine on 18-7-20.
//

#include <iostream>
#include <stack>
#include <queue>
#include "XmlLoader.h"
#include "Struct.h"

XmlLoader::XmlLoader() {

}

XmlLoader::~XmlLoader() {

}

bool XmlLoader::load() {
    if (!this->openXmlDoc()) {
        std::cout << "[XmlLoader::load]open xml document fail" << std::endl;
    }
    this->loadAllStruct();
    this->backpatchType();
    while (this->backpatchSize()); //循环回填
    this->setParentStructNode();
    this->createIndex();
    this->printAllStructEntry();
    return true;
}


int XmlLoader::getDataType(const char *typeName) {
    //todo update here
    if (strcmp(typeName, "block") == 0) {
        return _BLOCK;
    } else if (strcmp(typeName, "int") == 0) {
        return _INT;
    } else if (strcmp(typeName, "char") == 0) {
        return _CHAR;
    } else if (strcmp(typeName, "string") == 0) {
        return _STRING;
    } else if(strcmp(typeName, "char[]") == 0){
        return _CHAR_ARRAY;
    }else if (strcmp(typeName, "uint8_t") == 0) {
        return _UINT8_T;
    } else if (strcmp(typeName, "uint16_t") == 0) {
        return _UINT16_T;
    } else if (strcmp(typeName, "uint32_t") == 0) {
        return _UINT32_T;
    } else if (strcmp(typeName, "uint64_t") == 0) {
        return _UINT64_T;
    } else if (strcmp(typeName, "int8_t") == 0) {
        return _INT8_T;
    } else if (strcmp(typeName, "int16_t") == 0) {
        return _INT16_T;
    } else if (strcmp(typeName, "int32_t") == 0) {
        return _INT32_T;
    } else if (strcmp(typeName, "int64_t") == 0) {
        return _INT64_T;
    } else if (strcmp(typeName, "unsigned char") == 0) {
        return _UNSIGNED_CHAR;
    } else if (strcmp(typeName, "short") == 0) {
        return _SHORT;
    } else if (strcmp(typeName, "float") == 0) {
        return _FLOAT;
    } else if (strcmp(typeName, "double") == 0) {
        return _DOUBLE;
    } else {//获取已经读入的类型
        for (int i = 0; i < this->structList.size(); ++i) {
            if (strcmp(typeName, structList[i]->name) == 0) {
                return structList[i]->structTypeInfo->type;
            }
        }
        return -1;
    }
}

int XmlLoader::getDataSize(const int &type) {
    switch (type) {
        case _BLOCK:
            return 0;//block类型应从配置中获得
        case _INT:
            return 4;
        case _CHAR:
            return 1;
        case _STRING:
            return 0;//string应从配置中获取，该处代码仅作提示，不会运行至此处
        case _CHAR_ARRAY:
            return 0;//char[]应从配置中获取，该处代码仅作提示，不会运行至此处
        case _UINT8_T:
            return 1;
        case _UINT16_T:
            return 2;
        case _UINT32_T:
            return 4;
        case _UINT64_T:
            return 8;
        case _INT8_T:
            return 1;
        case _INT16_T:
            return 2;
        case _INT32_T:
            return 4;
        case _INT64_T:
            return 8;
        case _UNSIGNED_CHAR:
            return 1;
        case _SHORT:
            return 2; //diff in machine
        case _FLOAT:
            return 4;
        case _DOUBLE:
            return 8;
        default:
            for (int i = 0; i < this->structList.size(); ++i) {
                if (type == structList[i]->structTypeInfo->type) {
                    return structList[i]->structTypeInfo->size;
                }
            }
            return 0;//没有找到

    }
}

int XmlLoader::printAllStructEntry() {
    int level = 0;
    std::stack<std::pair<StructNode *, int>> structNodeStack;
    std::pair<StructNode *, int> structNode;
    for (int i = 0; i < this->structList.size(); ++i) {
        structNodeStack.push(std::make_pair(this->structList[i], level));
    }
    while (!structNodeStack.empty()) {
        structNode = structNodeStack.top();
        structNodeStack.pop();
        for (int j = 0; j < structNode.second; ++j) {
            std::cout << "-";
        }
        std::cout << "(" << structNode.first->name << ")[" << structNode.first->structTypeInfo->size << "]"
                  << std::endl;
        for (int i = 0; i < structNode.first->structNodeList.size(); ++i) {
            structNodeStack.push(std::make_pair(structNode.first->structNodeList[i], structNode.second + 1));
        }
    }
    return 0;
}

bool XmlLoader::openXmlDoc() {
    this->xmlError = this->xmlDocument.LoadFile(docPath);
    return (this->xmlError == tinyxml2::XMLError::XML_SUCCESS);
}


//todo 读入格式不正确如没有类型告警
bool XmlLoader::loadAllStruct() {
    tinyxml2::XMLElement *curStructElement;
    tinyxml2::XMLElement *curEntryElement;
    tinyxml2::XMLElement *rootElement;
    int autoSetType = AutoSetTypeStart;
    int arrayMemberCount = 0;
//    StructNode *preArrayMember = NULL;

    this->pBackpatchChainHead = new StructChain;
    this->pBackpatchChainHead->pNext = NULL;
    this->pBackpatchChainHead->pStructNode = NULL;

    rootElement = this->xmlDocument.RootElement();
    curStructElement = rootElement->FirstChildElement();
    while (true) {
        StructNode *tmpStructInfo = new StructNode;
        StructType *tmpStructTypeInfo = new StructType;
        tmpStructInfo->structTypeInfo = tmpStructTypeInfo;

        tmpStructInfo->name = curStructElement->Attribute("name");
        bzero(&tmpStructInfo->arrayInfo, sizeof(tmpStructInfo->arrayInfo));

        tmpStructInfo->structTypeInfo->typeName = curStructElement->Attribute("name");
        tmpStructInfo->structTypeInfo->type = autoSetType++;
        tmpStructInfo->structTypeInfo->size = -1;
        tmpStructInfo->parentStructNode = NULL;
        //添加到回填链等待后续回填size
        StructChain *pStructChain = new StructChain;
        StructChain *tmpStructChain = NULL;
        pStructChain->pStructNode = tmpStructInfo;
        tmpStructChain = this->pBackpatchChainHead->pNext;
        pStructChain->pNext = tmpStructChain;
        this->pBackpatchChainHead->pNext = pStructChain;

        curEntryElement = curStructElement->FirstChildElement();
        while (true) {
            //
            arrayMemberCount = curEntryElement->IntAttribute("count", 0);
            if (arrayMemberCount > 1) {
                //array按照数量进行直接展开，并进行标记
                for (int i = 0; i < arrayMemberCount; ++i) {
                    StructNode *tmpStructNode = new StructNode;
                    tmpStructNode->name = curEntryElement->Attribute("name");
                    tmpStructNode->arrayInfo.isArray = true;
                    tmpStructNode->arrayInfo.arrayIndex = i;
                    tmpStructNode->arrayInfo.count = arrayMemberCount;

                    StructType *tmpStructTypeInfo = new StructType;
                    tmpStructNode->structTypeInfo = tmpStructTypeInfo;

                    tmpStructNode->structTypeInfo->typeName = curEntryElement->Attribute("type");
                    tmpStructNode->structTypeInfo->size = curEntryElement->IntAttribute("size", -1);
                    //type和size等待回填
                    tmpStructNode->structTypeInfo->type = 0;
                    tmpStructNode->parentStructNode = NULL;

                    tmpStructInfo->structNodeList.push_back(tmpStructNode);
                }
            } else {
                StructNode *tmpStructNode = new StructNode;
                tmpStructNode->name = curEntryElement->Attribute("name");
                bzero(&tmpStructNode->arrayInfo, sizeof(tmpStructNode->arrayInfo));

                StructType *tmpStructTypeInfo = new StructType;
                tmpStructNode->structTypeInfo = tmpStructTypeInfo;

                tmpStructNode->structTypeInfo->typeName = curEntryElement->Attribute("type");
                tmpStructNode->structTypeInfo->size = curEntryElement->IntAttribute("size", -1);
                //type和size等待回填
                tmpStructNode->structTypeInfo->type = 0;
                tmpStructNode->parentStructNode = NULL;
                tmpStructInfo->structNodeList.push_back(tmpStructNode);
            }


            if (curEntryElement == curStructElement->LastChildElement()) {
                break;
            } else {
                curEntryElement = curEntryElement->NextSiblingElement();
            }
        }
        this->structList.push_back(tmpStructInfo);

        if (curStructElement == rootElement->LastChildElement()) {
            break;
        } else {
            curStructElement = curStructElement->NextSiblingElement();
        }
    }
    return true;
}

bool XmlLoader::backpatchType() {
    int tmpType = 0;
    for (int i = 0; i < this->structList.size(); ++i) {
        for (int j = 0; j < this->structList[i]->structNodeList.size(); ++j) {
            tmpType = getDataType(this->structList[i]->structNodeList[j]->structTypeInfo->typeName);
            if (tmpType != -1) {
                this->structList[i]->structNodeList[j]->structTypeInfo->type = tmpType;
            } else {
                std::cout << "[XmlLoader::backpatchType]no data structure type: " << this->structList[i]->structNodeList[j]->structTypeInfo->typeName << std::endl;
            }
        }
    }
}

bool XmlLoader::backpatchSize() {
    StructChain *pNode = pBackpatchChainHead->pNext;
    StructChain *preStructChain = pBackpatchChainHead;
    int size, tmpSize;
    bool continueLoop;
    while (pNode != NULL) {
        size = 0;
        continueLoop = false;
        for (int i = 0; i < pNode->pStructNode->structNodeList.size(); ++i) {
            tmpSize = pNode->pStructNode->structNodeList[i]->structTypeInfo->size;
            if (tmpSize < 0) {//set entry size
                tmpSize = this->getDataSize(pNode->pStructNode->structNodeList[i]->structTypeInfo->type);
                pNode->pStructNode->structNodeList[i]->structTypeInfo->size = tmpSize;
            }
            if (tmpSize >= 0) {
                size += tmpSize;
            } else {
                continueLoop = true;
                break;
            }
        }
        if (continueLoop) {
            preStructChain = pNode;
            pNode = pNode->pNext;
            continue;
        } else {
            //计算完成后赋值
            pNode->pStructNode->structTypeInfo->size = size;
            //将该结构挂载到所有entry里包含该结构的struct里 //
            this->mountStructIoEntry(pNode->pStructNode);
            //链上摘下
            preStructChain->pNext = pNode->pNext;
            delete pNode;
            return true;
        }
    }
    //失败或是已经回填完成
    if (this->pBackpatchChainHead->pNext == NULL) {
        return false;
    } else {
        std::cout << "[XmlLoader::backpatchSize]backpatch size false. please check if has [a in struct b, b in struct a]." << std::endl;
        exit(0);
    }
}

bool XmlLoader::mountStructIoEntry(StructNode *pStructNode) {
    StructChain *pNode = pBackpatchChainHead->pNext;
    while (pNode != NULL) {
        for (int i = 0; i < pNode->pStructNode->structNodeList.size(); ++i) {
            if (pNode->pStructNode->structNodeList[i]->structTypeInfo->type == pStructNode->structTypeInfo->type) {
                //释放原内存空间
                delete pNode->pStructNode->structNodeList[i]->structTypeInfo;
                //挂载
                pNode->pStructNode->structNodeList[i]->structTypeInfo = pStructNode->structTypeInfo;
                //拷贝
                this->deepCopy(pNode->pStructNode->structNodeList[i]->structNodeList, pStructNode->structNodeList);
            }
        }
        pNode = pNode->pNext;
    }
    return false;
}

//对于数组成员，直接返回第一个成员类型
int XmlLoader::getBasicType(const std::vector<char *> &columnNameList) {
    std::vector<StructNode *> *structNodeList = &this->structList;
    bool found;
    int type = 0;

    for (int i = 0; i < columnNameList.size(); ++i) {
        found = false;
        for (int j = 0; j < structNodeList->size(); ++j) {
            if (strcmp(columnNameList[i], (*structNodeList)[j]->name) == 0) {
                type = (*structNodeList)[j]->structTypeInfo->type;
                structNodeList = &(*structNodeList)[j]->structNodeList;
                found = true;
                break;
            }
        }
        if (!found) {
            return 0;
        }
    }

    if (type >= AutoSetTypeStart) {
        return 0;
    } else {
        return type;
    }
}

//这里要增加数组类型支持
int XmlLoader::getDataIndex(const std::vector<char *> &columnNameList) {
    //这里的定位较为简单，因为寻找是按照层级进行的
    std::vector<StructNode *> *structNodeList = &this->structList;
    bool found;
    int index = 0;

    for (int i = 0; i < columnNameList.size(); ++i) {
        found = false;
        for (int j = 0; j < structNodeList->size(); ++j) {
            if (strcmp(columnNameList[i], (*structNodeList)[j]->name) == 0) {
                structNodeList = &(*structNodeList)[j]->structNodeList;
                found = true;
                break;
            }
            index += (*structNodeList)[j]->structTypeInfo->size;
        }
        if (!found) {
            return 0;
        }
    }

    return index;
}

bool XmlLoader::createIndex() {
    //创建索引
    std::queue<StructNode *> structNodeQueue;
    int index = 0;
    StructNode *structNode;
    this->structList[0]->index = 0;
    structNodeQueue.push(this->structList[0]);
    while (!structNodeQueue.empty()) {
        structNode = structNodeQueue.front();
        structNodeQueue.pop();
        if (structNode->structTypeInfo->type < AutoSetTypeStart) {
            structNode->isLeaf = true;
        } else {
            structNode->isLeaf = false;
        }

        index = structNode->index;
        for (int i = 0; i < structNode->structNodeList.size(); ++i) {
            structNode->structNodeList[i]->index = index;
            index += structNode->structNodeList[i]->structTypeInfo->size;
            structNodeQueue.push(structNode->structNodeList[i]);
        }
    }
    return false;
}

void XmlLoader::deepCopy(std::vector<StructNode *> &destStructNodeList, const std::vector<StructNode *> &srcStructNodeList) {
    for (int i = 0; i < srcStructNodeList.size(); ++i) {
        StructNode *structNode = new StructNode;
        structNode->structNodeList.assign(srcStructNodeList[i]->structNodeList.begin(), srcStructNodeList[i]->structNodeList.end());
        structNode->structTypeInfo =srcStructNodeList[i]->structTypeInfo;
        structNode->index = srcStructNodeList[i]->index;
        structNode->name = srcStructNodeList[i]->name;
        structNode->arrayInfo = srcStructNodeList[i]->arrayInfo;
        structNode->isLeaf = srcStructNodeList[i]->isLeaf;
        structNode->parentStructNode = srcStructNodeList[i]->parentStructNode;
        destStructNodeList.push_back(structNode);
    }
}

bool XmlLoader::setParentStructNode() {
    std::queue<StructNode *> structNodeQueue;
    StructNode *tmpStructNode;
    structNodeQueue.push(this->structList[0]);
    while (!structNodeQueue.empty()){
        tmpStructNode = structNodeQueue.front();
        structNodeQueue.pop();
        for (int i = 0; i < tmpStructNode->structNodeList.size(); ++i) {
            structNodeQueue.push(tmpStructNode->structNodeList[i]);
            tmpStructNode->structNodeList[i]->parentStructNode = tmpStructNode;
        }
    }
    return false;
}

