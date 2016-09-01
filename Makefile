opt=-Wall -g
all: bin/tdj-config bin/tdj-localjudge
bin/tdj-config: build/tdj-config.o build/config.o
	cc build/tdj-config.o build/config.o -o bin/tdj-config $(opt)
build/tdj-config.o: src/config/tdj-config.c src/config/config.h
	cc -c src/config/tdj-config.c -o build/tdj-config.o $(opt)
build/config.o: src/config/config.c src/config/config.h
	cc -c src/config/config.c -o build/config.o $(opt)

bin/tdj-localjudge: build/tdj-localjudge.o build/judger.o build/compare.o build/config.o build/time.o
	cc build/tdj-localjudge.o build/judger.o build/compare.o build/config.o build/time.o -o bin/tdj-localjudge $(opt)
build/tdj-localjudge.o: src/config/config.h src/judger/judger.h src/judger/compare.h src/judger/tdj-localjudge.c src/time/time.h
	cc -c src/judger/tdj-localjudge.c -o build/tdj-localjudge.o $(opt)
build/judger.o: src/judger/judger.c src/judger/judger.h src/config/config.h
	cc -c src/judger/judger.c -o build/judger.o $(opt)
build/compare.o: src/judger/compare.c src/judger/compare.h
	cc -c src/judger/compare.c -o build/compare.o $(opt)

build/time.o: src/time/time.c src/time/time.h
	cc -c src/time/time.c -o build/time.o $(opt)

clean:
	-rm build/*.o
