//
// Created by sunshine on 18-7-16.
//

#ifndef INC_2018071603SHARE_MEM_ANALYSE_ANALYSE_H
#define INC_2018071603SHARE_MEM_ANALYSE_ANALYSE_H


#include <cstddef>
#include <string>
#include <vector>
#include "lib/tinyxml2.h"
#include "Struct.h"
#include "XmlLoader.h"
#include "MemManager.h"
#include "SqlAnalyse.h"


class ShareMemoryAnalyse {
public:
    ShareMemoryAnalyse();

    ~ShareMemoryAnalyse();

    bool loadXml(); //加载xml，并分析struct结构

    bool setAndLoadSql(); //设置并分析sql

    bool analyseSqlStruct(); //Sql结构分析

    bool linkMem(const int &shmid); //链接共享内存

    void setArgs();

    bool findData(const bool &isFirst);

    void recycleMemory(); //回收内存

private:
    XmlLoader *xmlLoader;
    SqlAnalyse *sqlAnalyse;
    MemManager *memManager;

    //error report
    void reportError(const int &level, const std::string &str);

    void getFirstInfo(const std::string &str);

};


#endif //INC_2018071603SHARE_MEM_ANALYSE_ANALYSE_H
