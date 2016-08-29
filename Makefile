all: bin/tdj-config
bin/tdj-config: build/tdj-config.o build/config.o
	cc build/tdj-config.o build/config.o -o bin/tdj-config -Wall
build/tdj-config.o: src/config/tdj-config.c src/config/config.h
	cc -c src/config/tdj-config.c -o build/tdj-config.o -Wall
build/config.o: src/config/config.c src/config/config.h
	cc -c src/config/config.c -o build/config.o -Wall
clean:
	-rm build/*.o
