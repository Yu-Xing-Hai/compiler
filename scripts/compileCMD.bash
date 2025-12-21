# 把 main.cpp 和 lexicalAnalysis.cpp 一起编译
g++ -Wall ../source/main.cpp ../source/lexicalAnalysis.cpp -I ../include/ -o ../build/main.exe && ../build/main.exe