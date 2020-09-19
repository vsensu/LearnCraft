# 将当前目录的所有源文件添加进变量
aux_source_directory(. DIRS_SRCS)

add_executable(test1 main.cpp demo.cpp)

include(CTest)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(test1)
