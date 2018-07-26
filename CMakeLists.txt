cmake_minimum_required(VERSION 3.8)
project(2018071603share_mem_analyse)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ShareMemoryAnalyse.cpp ShareMemoryAnalyse.h lib/tinyxml2.h lib/tinyxml2.cpp config/Config.h Struct.h XmlLoader.cpp XmlLoader.h MemManager.cpp MemManager.h SqlAnalyse.cpp SqlAnalyse.h)
add_executable(2018071603share_mem_analyse ${SOURCE_FILES})