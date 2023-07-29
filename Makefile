OBJ = main.o crypto_manager.o gather_manager.o session_manager.o \
	  structs_manager.o thread_manager.o

EXEWIN = silentic.exe
MAIN = src\main.c

MINGW = C:\MinGW

LIBSDWIN = -L$(MINGW)
INCSDWIN = -I$(MINGW)\include
LIBSWIN = -lws2_32

BINx32WIN = .\build\x32
BINx64WIN = .\build\x64


windows: silentic.exe

silentic.exe : $(objects)
	gcc $(MAIN) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWIN) -m32 $(LIBSWIN)
	gcc $(MAIN) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWIN) -m64 $(LIBSWIN)


session_manager.o : structs_manager.o crypto_manager.o
	gcc -c session_manager.h
