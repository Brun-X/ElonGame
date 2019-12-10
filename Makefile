all:
	g++ olcTest.cpp olcPixelGameEngine.h -o elon -lX11 -lGL -lpthread -lpng -lstdc++fs