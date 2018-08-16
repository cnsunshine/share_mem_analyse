//
// Created by sunshine on 18-7-20.
//

#include <iostream>
#include <cstring>
#include <queue>
#include "SqlAnalyse.h"

void SqlAnalyse::toLower() {
    for (int i = 0; i < sql.length(); ++i) {
        sql[i] = (char) tolower(sql[i]);
    }
}

SqlAnalyse::SqlAnalyse() {
    this->filename.clear();
    this->autoSetQueryId = 0;
}

SqlAnalyse::~SqlAnalyse() {

}

bool SqlAnalyse::isOperator(const char &ch) {
    return (ch == '>' || ch == '=' || ch == '<');
}

bool SqlAnalyse::isOperator(const std::string &text) {
    return (text == ">" || text == "=" || text == "<" || text == ">=" || text == "<=");
}

bool SqlAnalyse::isLetter(const char &ch) {
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

bool SqlAnalyse::isLetter(const std::string &text) {
    if (!(text[0] >= 'a' && text[0] <= 'z') && !(text[0] >= 'A' && text[0] <= 'Z')) {
        return false;
    }
    for (int i = 0; i < text.length(); ++i) {
        if ((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z') ||
            (text[i] >= '0' && text[i] <= '9')) {
            continue;
        } else {
            return false;
        }
    }

    return true;
}

bool SqlAnalyse::isString(const std::string &text) {
    for (int i = 0; i < text.length(); ++i) {
        if ((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z') ||
            (text[i] >= '0' && text[i] <= '9')) {
            continue;
        } else {
            return false;
        }
    }

    return true;
}

bool SqlAnalyse::isNumber(const char &ch) {
    return (ch >= '0' && ch <= '9');
}

bool SqlAnalyse::isNumber(const std::string &text) {
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }

    return true;
}

void SqlAnalyse::analyseTextStep1() {
    int i = 0;
    int status = 0;
    std::string text;
    while (i < this->sql.length()) {
        switch (status) {
            case 0:
                if (this->sql[i] == ' ') {
                    status = 0;
                    i++;
                } else if (this->isLetter(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    status = 1;
                    i++;
                } else if (this->isNumber(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    status = 2;
                    i++;
                } else if (isOperator(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    status = 3;
                    i++;
                } else if (this->sql[i] == ',') {
                    text.push_back(this->sql[i]);
                    status = 4;
                    i++;
                } else if (this->sql[i] == '(') {
                    text.push_back(this->sql[i]);
                    status = 5;
                    i++;
                } else if (this->sql[i] == ')') {
                    text.push_back(this->sql[i]);
                    status = 6;
                    i++;
                } else if (this->sql[i] == '\'') {
                    text.push_back(this->sql[i]);
                    status = 7;
                    i++;
                } else if (this->sql[i] == '*') {
                    text.push_back(this->sql[i]);
                    status = 11;
                    i++;
                } else {
                    status = 8;
                };
                break;
            case 1:
                if (this->isLetter(this->sql[i]) || this->isNumber(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    status = 1;
                    i++;
                } else if (this->sql[i] == '.') {
                    //todo 这里需要处理
                    text.push_back(this->sql[i]);
                    i++;
                } else {
                    status = 10;
                }
                break;
            case 2:
                if (this->isNumber(this->sql[i]) || this->isLetter(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    status = 2;
                    i++;
                } else {
                    status = 10;
                }
                break;
            case 3://operator
                if (isOperator(this->sql[i])) {
                    text.push_back(this->sql[i]);
                    i++;
                } else {
                    status = 9;
                }
                break;
            case 4:
            case 5:
            case 6:
            case 11:
                this->textList.push_back(text);
                status = 0;
                text = "";
                break;
            case 7:
                if (this->sql[i] == '\'') {
                    text.push_back(this->sql[i]);
                    status = 10;
                    i++;
                } else {
                    text.push_back(this->sql[i]);
                    i++;
                }
                break;
            case 8://error
                status = 0;
                text = "";
                break;
            case 9://operator check and push
                if (isOperator(text)) {
                    this->textList.push_back(text);
                    status = 0;
                    text = "";
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep1]bad sql: bad operator" << std::endl;
                    exit(0);
                }
                break;
            case 10://push text
                this->textList.push_back(text);
                status = 0;
                text = "";
                break;

        }
    }
    //last
    this->textList.push_back(text);
}

void SqlAnalyse::setAndLoadSql(const std::vector<StructNode *> &structList) {
    this->structList = structList;
    std::cout << "sql>";
//    std::cin.ignore(1024, '\n');
    getline(std::cin, this->sql);
    this->analyseTextStep1();
    this->analyseTextStep2();
    this->expandQueryCondition();
}

void SqlAnalyse::analyseTextStep2() {
    int status = 0;
    for (int i = 0; i < this->textList.size(); ++i) {
        switch (status) {
            case 0:
                if (this->textList[i] == "select") {
                    sqlType = 1;
                    status = 1;
                } else if (this->textList[i] == "update") {
                    sqlType = 3;
                    status = 3;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad start" << std::endl;
                    exit(0);
                }
                break;
            case 1:
                if (isLetter(this->textList[i])) {
//                    columnList.push_back({this->textList[i]});
                    //todo 暂时未实现
                    status = 11;//wait for ',' or 'from'
                } else if (this->textList[i] == "from") {
                    status = 12;
                } else if (this->textList[i] == "*") {
                    //todo 暂时未实现
                    status = 16;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                    exit(0);
                }
                break;
            case 16: //select *
                if (this->textList[i] == "from") {
                    this->filename.clear();
                    status = 12;
                } else if (this->textList[i] == "into") {
                    status = 17;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                    exit(0);
                }
                break;
            case 17:
                if (this->isLetter(this->textList[i])) {
                    this->filename = this->textList[i];
                    status = 18;
                }
                break;
            case 18:
                if (this->textList[i] == "from") {
                    status = 12;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                    exit(0);
                }
                break;
            case 11:
                if (this->textList[i] == ",") {
                    status = 1;
                    continue;
                } else if (this->textList[i] == "from") {
                    status = 12;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                    exit(0);
                }
                break;
            case 12://select ... from
                if (isLetter(this->textList[i])) {
                    this->tableName = this->textList[i];
                    status = 13;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: please input all" << std::endl;
                    exit(0);
                }
                break;
            case 13:
                if (this->textList[i] == "where") {
                    status = 14;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                    exit(0);
                }
                break;
            case 14:
                i += 2;
                if (i < this->textList.size()) {//一个查询需要三个
                    if ((isLetter(this->textList[i - 2]) || isColumn(this->textList[i - 2])) &&
                        isOperator(this->textList[i - 1]) &&
                        (isLetter(this->textList[i]) || isNumber(this->textList[i]) || isString(this->textList[i]))) {
                        queryConditionList.push_back(
                                this->buildQueryCondition(this->textList[i - 2], this->textList[i - 1],
                                                          this->textList[i]));
                        status = 15;
                    } else {
                        std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: bad text" << std::endl;
                        exit(0);
                    }
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: lost query condition" << std::endl;
                    exit(0);
                }
                break;
            case 15://寻找and或回退
                if (this->textList[i] == "and") {
                    status = 14;
                } else {
                    std::cout << "[SqlAnalyse::analyseTextStep2]bad sql sentence: expect a 'and'" << std::endl;
                    exit(0);
                }
                break;
//            case 3://update
//                if (isLetter(this->textList[i])) {
//                    dbTable = this->textList[i];
//                    status = 31;
//                } else {
//                    throw "bad sql sentence: lost table name";
//                }
//                break;
//            case 31:
//                if (this->textList[i] == "set") {
//                    status = 32;
//                } else {
//                    throw "bad sql sentence: respect a 'set'";
//                }
//                break;
//            case 32:
//                i += 2;
//                if (i < this->textList.size()) {
//                    if (isLetter(this->textList[i - 2]) && isOperator(this->textList[i - 1]) &&
//                        (isLetter(this->textList[i]) || isNumber(this->textList[i]))) {
//                        updateInfoList.push_back({this->textList[i - 2], this->textList[i - 1], this->textList[i]});
//                        status = 33;//wait for ',' or 'where'
//                    } else {
//                        throw "bad sql sentence: bad text";
//                    }
//                } else {
//                    throw "bad sql sentence: lost update condition";
//                }
//                break;
//            case 33:
//                if (this->textList[i] == ",") {
//                    status = 32;
//                } else if (this->textList[i] == "where") {
//                    status = 34;
//                } else {
//                    throw "bad sql sentence: lost ',' or 'where'";
//                }
//                break;
//            case 34:
//                i += 2;
//                if (i < this->textList.size()) {//一个查询需要三个
//                    if (isLetter(this->textList[i - 2]) && isOperator(this->textList[i - 1]) &&
//                        (isLetter(this->textList[i]) || isNumber(this->textList[i]))) {
//                        queryConditionList.push_back({this->textList[i - 2], this->textList[i - 1], this->textList[i]});
//                        status = 35;
//                    } else {
//                        throw "bad sql sentence: bad text";
//                    }
//                } else {
//                    throw "bad sql sentence: lost query condition";
//                }
//                break;
//            case 35://寻找and或回退
//                if (this->textList[i] == "and") {
//                    status = 34;
//                } else {
//                    throw "bad sql sentence: expect a 'and'";
//                }
//                break;
            default:
                break;
        }
    }
}

bool SqlAnalyse::isColumn(const std::string &text) {
    for (int i = 0; i < text.length(); ++i) {
        if (!((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z') || text[i] == '.')) {
            return false;
        }
    }
    return true;
}

QueryCondition *
SqlAnalyse::buildQueryCondition(const std::string &columnName, const std::string &op, const std::string &value) {
    std::string tmpName;
    QueryCondition *queryCondition = new QueryCondition;
    for (int i = 0; i < columnName.length(); ++i) {
        if (columnName[i] != '.') {
            tmpName.push_back(columnName[i]);
        } else {
            //申请heap内存存放
            char *str = new char[tmpName.length() + 1];
            memset(str, 0, tmpName.length() + 1);
            strcpy(str, tmpName.c_str());
            queryCondition->columnNameList.push_back(str);
            tmpName.clear();
        }
    }
    if (tmpName.length() > 0) {
        //申请heap内存存放
        char *str = new char[tmpName.length() + 1];
        memset(str, 0, tmpName.length() + 1);
        strcpy(str, tmpName.c_str());
        queryCondition->columnNameList.push_back(str);
        tmpName.clear();
    }

    queryCondition->value = value.c_str();
    queryCondition->op = op;
    queryCondition->basicType = 0;
    queryCondition->index = 0;
    queryCondition->structNode = NULL;
    queryCondition->queryId = this->autoSetQueryId++;
    return queryCondition;
}


bool SqlAnalyse::expandQueryCondition() {
    //初始化
    //设置查询总数
    this->queryConditionTotalNum = (int) this->queryConditionList.size();

    std::vector<char *>::iterator it;
    for (int i = 0; i < this->queryConditionList.size(); ++i) {
        it = this->queryConditionList[i]->columnNameList.begin();
        if (strcmp((*it), this->structList[0]->name) == 0) {
            this->queryConditionList[i]->structNode = this->structList[0]; //structList[0]为主结构
            this->queryConditionList[i]->columnNameList.erase(it);
        } else {
            std::cout << "[SqlAnalyse::expandQueryCondition]input first struct wrong" << std::endl;
            exit(0);
        }
    }
    //循环下降
    while (true) {
        bool change = false;
        for (int i = 0; i < queryConditionList.size(); ++i) {
            std::vector<char *>::iterator iterator_columnName;
            if (this->queryConditionList[i]->columnNameList.empty()) {
                continue;
            }
            iterator_columnName = this->queryConditionList[i]->columnNameList.begin();
            bool match = false;
            for (int j = 0; j < this->queryConditionList[i]->structNode->structNodeList.size(); ++j) {
                if (strcmp(this->queryConditionList[i]->structNode->structNodeList[j]->name, (*iterator_columnName)) ==
                    0) {
                    QueryCondition *queryCondition = new QueryCondition;
                    queryCondition->columnNameList = this->queryConditionList[i]->columnNameList;
                    queryCondition->columnNameList.erase(queryCondition->columnNameList.begin()); //摘除头字符串，已经匹配
                    queryCondition->basicType = this->queryConditionList[i]->structNode->structNodeList[j]->structTypeInfo->type;
                    queryCondition->index = this->queryConditionList[i]->structNode->structNodeList[j]->index;
                    queryCondition->structNode = this->queryConditionList[i]->structNode->structNodeList[j];
                    queryCondition->op = this->queryConditionList[i]->op;
                    queryCondition->value = this->queryConditionList[i]->value;
                    queryCondition->queryId = this->queryConditionList[i]->queryId;
                    //todo 释放QueryCondition
                    this->queryConditionList.push_back(queryCondition);
                    match = true; //匹配到
                    change = true; //有更改
                }
            }
            this->queryConditionList.erase(this->queryConditionList.begin() + i); //删除已经更新的情况
            if (!match) {
                std::cout << "[SqlAnalyse::expandQueryCondition]match wrong" << std::endl;
                exit(0);
            }

        }
        if (change) {
            continue;
        } else {
            return true;
        }
    }

}

void SqlAnalyse::recycleMemory() {
    this->sql.clear();
    this->textList.clear();
    this->sqlType = 0;
    this->tableName.clear();
    this->autoSetQueryId = 0;

    this->queryConditionTotalNum = 0;
    for (int i = 0; i < this->queryConditionList.size(); ++i) {
        for (int j = 0; j < this->queryConditionList[i]->columnNameList.size(); ++j) {
            delete this->queryConditionList[i]->columnNameList[i];
        }
        delete this->queryConditionList[i];
    }
    this->queryConditionList.clear();
}
