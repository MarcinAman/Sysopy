all: main.c
	gcc -Wall -lpthread -o main main.c
clean:
	rm -f main
test:
	./main 1 a.pgm f.txt f.pgm >> result.txt
	./main 2 a.pgm f.txt f.pgm >> result.txt
	./main 4 a.pgm f.txt f.pgm >> result.txt
	./main 8 a.pgm f.txt f.pgm >> result.txt
