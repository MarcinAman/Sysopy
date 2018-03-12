all: compile static dynamic

compile: DynamicPointerArray.c StaticArray.c
	gcc -std=c99 -c DynamicPointerArray.c StaticArray.c
	gcc -std=c99 -fPIC -c -o DynamicPointerArray2.o DynamicPointerArray.c
	gcc -std=c99 -fPIC -c -o StaticArray2.o StaticArray.c
static:
	ar -cr libstatarray.a DynamicPointerArray.o StaticArray.o
shared:
	gcc -std=c99 -shared -o libdynarray.so DynamicPointerArray2.o StaticArray2.o
clean:
	rm -f *o *.so *.a
main_static:
	gcc -c main.c
	gcc main.o libstatarray.a -o main
	rm -f raport.txt
	./main >> raport.txt
main_shared:
	gcc -L`pwd` main.c -libdynarray
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc main.c -l libdynarray -L . -o main
