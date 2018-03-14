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
	rm -f *o *.so *.a *.out
	rm -f main
	rm -f static_main
main_static: compile static
	gcc -std=c99 -c -o static_main.o static_main.c -O$(lvl)
	gcc -std=c99 static_main.o libstatarray.a -o static_main -O$(lvl)
	#rm -f raport.txt
main_shared: compile shared
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc -std=c99 -o main static_main.c -L. libdynarray.so -O$(lvl)
dynamic: compile shared
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc -std=c99 -L. -o main main.c libdynarray.so -ldl -O$(lvl)


