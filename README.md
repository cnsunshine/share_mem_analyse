> 该工具是用来连接共享内存并进行编译调试
<!-- TOC -->

- [1. version 1.0](#1-version-10)
    - [1.1. 介绍](#11-介绍)
        - [1.1.1. ShareMemoryAnalyse](#111-sharememoryanalyse)
        - [1.1.2. XmlLoader](#112-xmlloader)
        - [1.1.3. SqlAnalyse](#113-sqlanalyse)
        - [1.1.4. MemManager](#114-memmanager)
    - [1.2. 使用](#12-使用)
        - [1.2.1. Config.h XML描述文件路径配置](#121-configh-xml描述文件路径配置)
        - [1.2.2. 编译](#122-编译)
        - [1.2.3. 运行](#123-运行)
    - [1.3. example](#13-example)
        - [1.3.1. StructInfo.xml](#131-structinfoxml)
        - [1.3.2. 操作](#132-操作)
    - [1.4. 已提供的功能](#14-已提供的功能)
        - [1.4.1. 支持的数据类型](#141-支持的数据类型)
        - [1.4.2. 支持的功能](#142-支持的功能)
    - [1.5. 即将更新的功能](#15-即将更新的功能)

<!-- /TOC -->
# 1. version 1.0
## 1.1. 介绍
### 1.1.1. ShareMemoryAnalyse
ShareMemoryAnalyse负责管理和调用各组件类，完成共享内存查看整个操作。
### 1.1.2. XmlLoader
XmlLoader根据config/Config.h里的配置加载定义了数据结构的XML文件，将主结构树加载到内存。
### 1.1.3. SqlAnalyse
SqlAnalyse用于实现对类SQL语句的词法、语法分析。
### 1.1.4. MemManager
MemManager对内存进行查询和输出。

## 1.2. 使用
### 1.2.1. Config.h XML描述文件路径配置
```c++
static const char * docPath = "/data/home/tabzzhang/code/share_mem_analyse/config/structInfo.xml";
```
### 1.2.2. 编译
```bash
cmake .
make
```
### 1.2.3. 运行
1. 键入共享内存shmid
2. 键入共享内存起始偏移位置。（例如一段共享内存，前面是100个固定分配的用户信息结构体，后面有100个固定分配的物品信息结构体。如果我们要查询某个固定物品，需要把起始的定位设置为`100*sizeof(用户信息结构体)`）。
3. 输入查询语句，查询语句目前仅支持一类 `select * [into _filename] from all where _FirstName._SecondName[.ThirdName...] [and ...]`  
*[]为可以缺省的内容*  
_filename : 将查询结果写入_filename文件中  
_FirstName/_SecondName/... : 为需要查询的内容名，以`.`符号递进。

## 1.3. example
### 1.3.1. StructInfo.xml
```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
    <!-- 内存块结构定义 -->
    <struct name="RoleInfo">
        <!-- block 类型用来标记跳过 -->
        <entry name="block" type="block" size="0"/>
        <entry name="age" type="int"/>
        <entry name="ch" type="char"/>
        <!-- string 类型必须指定长度 -->
        <entry name="str" size="100" type="string"/>
        <entry name="student" type="StudentInfo"/>
        <entry name="location" type="UserLocation"/>
    </struct>

    <!--自定义数据结构声明-->
    <struct name="StudentInfo">
        <entry name="id" type="int"/>
    </struct>
    <struct name="UserLocation">
        <entry name="loc" type="CommonLocation"/>
        <entry name="description" type="string" size="255"/>
    </struct>
    <struct name="CommonLocation">
        <entry name="country" type="string" size="50"/>
        <entry name="province" type="string" size="20"/>
    </struct>
</root>
```
### 1.3.2. 操作
```
input shmid: 3375144
start load xml config for data structure.
(CommonLocation)[70]
-(province)[20]
-(country)[50]
(UserLocation)[325]
-(description)[255]
-(loc)[70]
--(province)[20]
--(country)[50]
(StudentInfo)[4]
-(id)[4]
(RoleInfo)[434]
-(location)[325]
--(description)[255]
--(loc)[70]
---(province)[20]
---(country)[50]
-(student)[4]
--(id)[4]
-(str)[100]
-(ch)[1]
-(age)[4]
-(block)[0]
input mem offset(查询的数据段的首条数据在内存中的起始位置): 0                              
sql>select * from all where RoleInfo.age = 5555
age: 5555
ch: d
str: dgefe
id: 1333
description: ged
country: chin
province: gaada
```

## 1.4. 已提供的功能
### 1.4.1. 支持的数据类型
```
int
char
char*(string)
char[],
uint8_t,
uint16_t,
uint32_t,
uint64_t
int8_t,
int16_t,
int32_t,
int64_t,
unsigned char,
short,
float,
double,
```
### 1.4.2. 支持的功能
* `=`条件（多组合条件）
* 单条复合条件的数据查询
* 结果写入文本
* 支持数组（含字符数组）
## 1.5. 即将更新的功能
* 支持update
* 支持更丰富的查询条件
* 支持指定字段输出