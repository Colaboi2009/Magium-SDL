#all: android compile play
all: android

android:
	./android.sh

#used to compile for windows and linux.
#incomplete, wont work yet!
compile:
	gcc -I../SDL/include -I../SDL_ttf/include -I../SDL_image -Linclude/lib -o main main.cpp -lmingw32 -lSDL3main -lSDL3 -lSDL3_ttf
play:
	.\main.exe
