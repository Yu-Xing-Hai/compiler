# 把 main.cpp 和 lexicalAnalysis.cpp 一起编译（关键：两个 .cpp 都要加）
g++ -Wall ../source/main.cpp ../source/lexicalAnalysis.cpp -I ../include/ -o ../build/main.exe && ../build/main.exe