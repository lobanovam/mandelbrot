all:
	g++ -c mandelbr.cpp -o mandelbr.o
	g++ mandelbr.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./sfml-app

clear:
	rm -f *.o