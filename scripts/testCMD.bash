# 编译测试函数，验证词法分析正确性
g++ -Wall ../test/lexicalAnalysisTest.cpp ../source/lexicalAnalysis.cpp -I ../include/ -o ../build/test.exe && ../build/test.exe