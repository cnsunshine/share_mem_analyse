//
// Created by sunshine on 18-7-20.
//

#ifndef INC_2018071603SHARE_MEM_ANALYSE_XMLLOADER_H
#define INC_2018071603SHARE_MEM_ANALYSE_XMLLOADER_H


#include <vector>
#include "Struct.h"
#include "lib/tinyxml2.h"
#include "config/Config.h"

class XmlLoader {
public:
    std::vector<StructNode *> structList;


    XmlLoader();

    ~XmlLoader();

    bool load();

    //获取基础类型，错误返回0，传入为多层地址 a.b
    int getBasicType(const std::vector<char *> &columnNameList);
    //获取数据起始下标，错误返回0，传入为多层地址 a.b
    int getDataIndex(const std::vector<char *> &columnNameList);

private:
    //tiny文档
    tinyxml2::XMLDocument xmlDocument;
    tinyxml2::XMLError xmlError;
    //等待回填size的链
    StructChain *pBackpatchChainHead;


    //打开xml文件
    bool openXmlDoc();
    //预加载所有数据结构
    bool loadAllStruct();
    //回填数据类型
    bool backpatchType();
    //回填数据尺寸
    bool backpatchSize();
    //挂载struct信息到回填链中的entry
    bool mountStructIoEntry(StructNode *pStructNode);
    //生成index数据
    bool createIndex();
    //获取数据类型
    int getDataType(const char *typeName);
    //获取数据尺寸
    int getDataSize(const int &type);
    //列出所有数据结构，并进行展开
    int printAllStructEntry();
};


#endif //INC_2018071603SHARE_MEM_ANALYSE_XMLLOADER_H
