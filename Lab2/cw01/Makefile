compile: SysOperation.c LibOperation.c
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc -std=c99 -c SysOperation.c
	gcc -std=c99 -c LibOperation.c
	ar -cr libfileoperation.a SysOperation.o LibOperation.o
	gcc -c -o main.o main.c
	gcc main.o libfileoperation.a -o main
	./main file.txt 20 10 >> debug.txt
clean:
	rm -f main *.o *.a copy.txt file.txt debug.txt
remove:
	rm -f copy.txt file.txt
