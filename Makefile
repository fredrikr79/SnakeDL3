compile:
	gcc -W -Wall -Wextra -pedantic -o out/main src/main.c -Iinclude -lpthread -Llib -lSDL3


run:
	make compile	
	./out/main
