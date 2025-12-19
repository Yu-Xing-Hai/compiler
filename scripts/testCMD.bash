# 编译测试函数，验证词法分析正确性
g++ -Wall ../source/lexicalAnalysis.cpp -Wall ../source/grammaticalAnalysis.cpp -Wall ../test/compilerTest.cpp -I ../include/ -o ../build/test.exe && ../build/test.exe