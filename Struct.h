//
// Created by sunshine on 18-7-17.
//

#ifndef INC_2018071603SHARE_MEM_ANALYSE_STRUCT_H
#define INC_2018071603SHARE_MEM_ANALYSE_STRUCT_H

#pragma pack(1)

//BEGIN report level
#define INFO 1
#define LEVEL 1
//END report level

enum DATATYPE {
    _BLOCK = 0,//该类型直接跳过打印
    _INT,//int
    _CHAR,//char
    _STRING,//char*(string)
    _UINT8_T,
    _UINT16_T,
    _UINT32_T,
    _INT8_T,
    _INT16_T,
    _INT32_T,
    _UNSIGNED_CHAR,
    _SHORT,
    _FLOAT,
    _DOUBLE,
};

struct StructType {
    const char *typeName;
    int size;
    int type;
};

struct StructNode {
    const char *name;
    bool isLeaf; //是否为叶子节点
    int index;
    StructType *structTypeInfo;
    std::vector<StructNode *> structNodeList;
};

//回填链
struct StructChain {
    StructNode *pStructNode;
    StructChain *pNext;
};

struct QueryCondition {
    std::vector<char *> columnNameList;
    int basicType;
    int index;
    std::string op;
    const char *value;
};

//打印节点
struct PrintNode {
    const char *name;
    int type;
    int index; //打印起始位置
};

#pragma pack()
#endif //INC_2018071603SHARE_MEM_ANALYSE_STRUCT_H
