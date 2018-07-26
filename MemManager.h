//
// Created by sunshine on 18-7-20.
//

#ifndef INC_2018071603SHARE_MEM_ANALYSE_MEMLINKER_H
#define INC_2018071603SHARE_MEM_ANALYSE_MEMLINKER_H


#include <cstddef>
#include <vector>
#include "Struct.h"

class MemManager {
public:
    MemManager();

    ~MemManager();

    bool linkMem(const int &shmid);

    void setShmOffset(int &offset);//设置查询的起始位置

    void setOutputFilename(const std::string &filename);

    void setStructList(std::vector<StructNode *> &structList);

    void setQueryConditionList(std::vector<QueryCondition *> &queryConditionList);

    void setStructSize(const int &size);

    bool findData(const bool &isFirst);

private:
    //****************结构数据**************//
    std::vector<StructNode *> structList;
    std::vector<QueryCondition *> queryConditionList; //查询条件
    std::vector<PrintNode> printNodeList;
    int structSize;
    std::string filename;


    //共享内存指针
    void *shm;//共享内存基址
    void *curShm;//当前指针
    size_t shmSize;

    //*************data input**************//
    //以下数据可以缩短，如intN_t(N=8,16,32,64)可用int64_t代替，使用时进行转换即可。
    int iData;
    char cData;
    char *strData;
    int8_t int8Data;
    int16_t int16Data;
    int32_t int32Data;
    int64_t int64Data;
    uint8_t uint8Data;
    uint16_t uint16Data;
    uint32_t uint32Data;
    uint64_t uint64Data;
    unsigned char ucData;
    short siData;
    float fData;
    double dData;


    //进程挂载上共享内存
    bool attachMem(const int &shmid);
    //进程离开共享内存
    bool leaveMem();
    //判断相等
    bool isEqual(void *lh, void *rh, const int &dataType);
    //offset偏移到下一个struct块
    void nextStructBlock();
    //从头搜索
    bool search(const bool &isFirst);
    //打印数据
    bool printData();

    bool printDataToFile();

    bool printByType(void *loc, const int &type);
    bool printByType(void *loc, const int &type, std::ofstream &outFile);
};


#endif //INC_2018071603SHARE_MEM_ANALYSE_MEMLINKER_H
