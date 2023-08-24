OBJ = main.o Server.o
OBJCLIENT = Client.o

EXEWIN = silentic.exe
EXEWINCLIENT = silent_client.exe
MAIN = src\main.c
MAINCLIENT = src\Client.c

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


client:
	gcc $(MAINCLIENT) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWINCLIENT) -m32 $(LIBSWIN)
	gcc $(MAINCLIENT) $(INCSDWIN) $(LIBSDWIN) -o $(BINx32WIN)\$(EXEWINCLIENT) -m64 $(LIBSWIN)

env:
	mkdir build
	mkdir build\x32
	mkdir build\x64

clean:
	rm -r build
	rm .\*.o