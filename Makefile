main: ./src/main.c
	gcc -g -o bin/cuppu -std=c89 ./src/*.c

clean:
	rm ./bin/cuppu
