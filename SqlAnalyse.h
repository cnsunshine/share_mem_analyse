//
// Created by sunshine on 18-7-20.
//

#ifndef INC_2018071603SHARE_MEM_ANALYSE_SQLANALYSE_H
#define INC_2018071603SHARE_MEM_ANALYSE_SQLANALYSE_H

#include <string>
#include <vector>
#include "Struct.h"

class SqlAnalyse {
public:
    std::vector<QueryCondition *> queryConditionList; //查询条件
    std::string filename;


    SqlAnalyse();
    ~SqlAnalyse();

    void setAndLoadSql();
private:
    std::string sql; //输入的sql语句
    std::vector<std::string> textList; //词法分析结果
    int sqlType; //SQL语句类型,1.select,2.insert,3.update,4.delete
    std::string tableName;

    void analyseTextStep1(); //词法分析
    void analyseTextStep2(); //语法分析

    void toLower(); //将原始sql语句转换为小写
    bool isOperator(const char &ch);
    bool isOperator(const std::string &text);
    bool isLetter(const char &ch);
    bool isLetter(const std::string &text);
    bool isNumber(const char &ch);
    bool isNumber(const std::string &text);
    bool isColumn(const std::string &text); //是否为查询列
    QueryCondition * buildQueryCondition(const std::string &columnName, const std::string &op, const std::string &value);
};


#endif //INC_2018071603SHARE_MEM_ANALYSE_SQLANALYSE_H
