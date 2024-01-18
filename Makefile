#Files
S_Files = source/main.cpp

#Executables
EXEC = main

#Compiler
CC = g++

#Compiler Flags
CXXFLAGS = -w -g

#Linker Flags
LXXFLAGS = -I src/include -I source -L src/lib -lmingw32 -lSDL2main -lSDL2 


# all:$(S_Files)
# 	$(CC) -o $(EXEC) $(S_Files) $(CXXFLAGS) $(LXXFLAGS)


main: main.o platform.o chip8.o
	$(CC) -o $(EXEC) $(S_Files) $(CXXFLAGS) $(LXXFLAGS)


main.o: source/main.cpp source/platform.h source/chip8.h
	$(CC) -c source/main.cpp $(CXXFLAGS) $(LXXFLAGS)	


platform.o: source/platform.h

chip8.o: source/chip8.h



