# 将当前目录的所有源文件添加进变量
aux_source_directory(. DIRS_SRCS)

#add_executable(tests main.cpp demo.cpp)
#
#add_test(Tests tests)
#
#enable_testing()

add_library(test_main OBJECT main.cpp)

add_executable(test1 demo.cpp $<TARGET_OBJECTS:test_main>)
add_test(NAME test1 COMMAND test1)

#add_executable(test2 test2.cpp $<TARGET_OBJECTS:test_main>)
#add_test(NAME test2 COMMAND test2 )