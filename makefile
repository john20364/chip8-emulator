CXX = g++
SDL_LIB = -LC:/SDL/SDL2-2.0.4-mingw/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 
SDL_INCLUDE = -IC:/SDL/SDL2-2.0.4-mingw/x86_64-w64-mingw32/include 
CFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE) 
LDFLAGS = -lmingw32 -mwindows -mconsole $(SDL_LIB) 
OBJS = main.o chip8.o
EXE = chip8.exe

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS)

main.o: main.cpp chip8.h
	$(CXX) main.cpp $(CFLAGS)

chip8.o: chip8.cpp chip8.h
	$(CXX) chip8.cpp $(CFLAGS)

.PHONY: clean
clean:
	del *.o && del $(EXE)
