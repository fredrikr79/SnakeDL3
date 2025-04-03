clean:
	rm -rf out/
	mkdir out/

compile:
	gcc -W -Wall -Wextra -pedantic -o out/main src/main.c src/utils/arena.c src/utils/vector.c -Iinclude -Iutils -lpthread -Llib -lSDL3

debug:
	gcc -W -Wall -Wextra -pedantic -g -O0 -o out/main_debug src/main.c src/utils/arena.c src/utils/vector.c -Iinclude -Iutils -lpthread -Llib -lSDL3

run:
	make compile	
	./out/main
