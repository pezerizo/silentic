set LIB_DIR=-LC:\MinGW\lib
set INC_DIR=-IC:\MinGW\include -IC:\Users\vlado\Desktop\projects\openssl\include

set MAIN_FILE=.\src\main.c
set BUILD_DIRx64=.\build\x64
set BUILD_DIRx32=.\build\x32

set APP_NAME=silentic.exe

mkdir %BUILD_DIRx64%
mkdir %BUILD_DIRx32%

gcc %MAIN_FILE% %INC_DIR% %LIB_DIR% -o %BUILD_DIRx32%\%APP_NAME% -m32 -lws2_32
gcc %MAIN_FILE% %INC_DIR% %LIB_DIR% -o %BUILD_DIRx64%\%APP_NAME% -m64
