all: main.c
	gcc -g -Wall -lpthread -o main main.c
clean:
	rm -f main out.txt
test: all
	./main conf.txt >> out.txt
