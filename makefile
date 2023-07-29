CFLAGS = -Wno-implicit-function-declaration

all:final

final: main.o search.o
	echo "Linking and producing the final application"
	g++ -o main.exe main.o -L. -l search
	g++ -c cpp/main.cpp -o main.o

main.o: cpp/main.cpp
	echo "Compiling the main file"
	g++ $(CFLAGS) -c cpp/main.cpp


clean:
	echo "Removing everything but the source files"
	del *.o