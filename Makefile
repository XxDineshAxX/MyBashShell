mysh: mysh.o
	gcc -std=c99 -o mysh mysh.c

mysh.o: mysh.c
	gcc -std=c99 -c mysh.c
