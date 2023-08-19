OBJ = main.o Server.o

EXEWIN = silentic.exe
MAIN = src\main.c

MINGW = C:\MinGW

LIBSDWIN = -L$(MINGW)\lib
INCSDWIN = -I$(MINGW)\include
LIBSWIN = -lws2_32

BINx32WIN = .\build\x32
BINx64WIN = .\build\x64


windows: silentic.exe

silentic.exe : $(objects)
	gcc $(MAIN) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWIN) -m32 $(LIBSWIN)
	gcc $(MAIN) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWIN) -m64 $(LIBSWIN)

Server.o : 
	gcc -c Server.c -o Server.o
