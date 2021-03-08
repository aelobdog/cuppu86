main: ./src/main.c
	gcc -g -o bin/cuppu -std=c89 ./src/*.c

small: ./src/main.c
	gcc -Os -s -o bin/cuppu_small -std=c89 ./src/*.c

tests: ./tests/tests.c
	cd tests ; gcc -c ../src/*.c ; rm main.o ; gcc * -o ../bin/tests

clean:
	rm ./bin/*

run:
	./bin/cuppu

runtests:
	./bin/tests
