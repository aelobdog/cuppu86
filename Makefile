main: ./src/main.c
	gcc -g -o bin/cuppu -std=c89 ./src/*.c

small: ./src/main.c
	gcc -Os -s -o bin/cuppu_small -std=c89 ./src/*.c

clean:
	rm ./bin/*
