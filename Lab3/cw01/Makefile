compile: main.c
	make clean
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc -std=c99 -c CatalogDisplay.c
	ar -cr libfileoperation.a CatalogDisplay.o
	gcc -c -o main.o main.c
	gcc main.o libfileoperation.a -o main
	echo "arguments: <path> <sign> <year> <month> <day> <int is_sys>"
clean:
	rm -f main *.o *.a
