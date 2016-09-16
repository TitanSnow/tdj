opt=-Wall -O2
all: bin/tdj-config bin/tdj-server bin/tdj-broadcast bin/tdj-new bin/tdj-push bin/tdj-listener
bin/tdj-config: build/tdj-config.o build/config.o build/sqlite3.o
	gcc build/tdj-config.o build/config.o build/sqlite3.o -o bin/tdj-config -lpthread -ldl $(opt)
build/tdj-config.o: src/config/tdj-config.c src/config/config.h
	gcc -c src/config/tdj-config.c -o build/tdj-config.o $(opt)
build/config.o: src/config/config.c src/config/config.h lib/sqlite/sqlite3.h
	gcc -c src/config/config.c -o build/config.o $(opt)

bin/tdj-localjudge: build/tdj-localjudge.o build/judger.o build/compare.o build/config.o build/time.o build/sqlite3.o
	gcc build/tdj-localjudge.o build/judger.o build/compare.o build/config.o build/time.o build/sqlite3.o -o bin/tdj-localjudge -lpthread -ldl $(opt)
build/tdj-localjudge.o: src/config/config.h src/judger/judger.h src/judger/compare.h src/judger/tdj-localjudge.c src/time/time.h
	gcc -c src/judger/tdj-localjudge.c -o build/tdj-localjudge.o $(opt)
build/judger.o: src/judger/judger.c src/judger/judger.h src/config/config.h
	gcc -c src/judger/judger.c -o build/judger.o $(opt)
build/compare.o: src/judger/compare.c src/judger/compare.h
	gcc -c src/judger/compare.c -o build/compare.o $(opt)

build/time.o: src/time/time.c src/time/time.h
	gcc -c src/time/time.c -o build/time.o $(opt)

bin/tdj-server: build/tdj-server.o build/config.o build/judger.o build/time.o build/compare.o build/sqlite3.o
	gcc build/tdj-server.o build/config.o build/judger.o build/time.o build/compare.o build/sqlite3.o -o bin/tdj-server -lpthread -ldl $(opt)
build/tdj-server.o: src/server/tdj-server.c src/server/server_def.h src/judger/judger.h src/judger/compare.h src/time/time.h src/config/config.h
	gcc -c src/server/tdj-server.c -o build/tdj-server.o -DNO_COMPILER_OUTPUT $(opt)

bin/tdj-broadcast: build/tdj-broadcast.o build/config.o build/sqlite3.o
	gcc build/tdj-broadcast.o build/config.o build/sqlite3.o -o bin/tdj-broadcast -lpthread -ldl $(opt)
build/tdj-broadcast.o: src/server/server_def.h src/config/config.h src/server/tdj-broadcast.c
	gcc -c src/server/tdj-broadcast.c -o build/tdj-broadcast.o $(opt)

bin/tdj-new: build/tdj-new.o
	gcc build/tdj-new.o -o bin/tdj-new $(opt)
build/tdj-new.o: src/client/tdj-new.c
	gcc -c src/client/tdj-new.c -o build/tdj-new.o $(opt)

bin/tdj-push: build/tdj-push.o build/time.o build/config.o build/sqlite3.o
	gcc build/tdj-push.o build/time.o build/config.o build/sqlite3.o -o bin/tdj-push -lpthread -ldl $(opt)
build/tdj-push.o: src/client/tdj-push.c src/time/time.h src/config/config.h src/server/server_def.h src/judger/judger.h
	gcc -c src/client/tdj-push.c -o build/tdj-push.o $(opt)

bin/tdj-listener: build/tdj-listener.o build/config.o build/sqlite3.o
	g++ build/tdj-listener.o build/config.o build/sqlite3.o -o bin/tdj-listener -lpthread -ldl $(opt)
build/tdj-listener.o: src/server/tdj-listener.cc src/server/server_def.h src/judger/judger.h src/config/config.h src/time/time.h
	g++ -c src/server/tdj-listener.cc -o build/tdj-listener.o $(opt)

build/sqlite3.o: lib/sqlite/sqlite3.c
	gcc -c lib/sqlite/sqlite3.c -o build/sqlite3.o $(opt)

clean:
	-rm build/*.o
prepare:
	-mkdir build
	-mkdir bin
