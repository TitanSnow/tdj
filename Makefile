opt =-Wall -O2
sopt=-Wall -O2 -Isrc -I.
all: bin/tdj-config bin/tdj-server bin/tdj-broadcast bin/tdj-new bin/tdj-push bin/tdj-listener
bin/tdj-config: build/tdj-config.o build/libconfig.a
	cc build/tdj-config.o build/libconfig.a -o bin/tdj-config $(opt)
build/tdj-config.o: src/config/tdj-config.c src/config/config.h
	cc -c src/config/tdj-config.c -o build/tdj-config.o $(sopt)
build/config.o: src/config/config.c src/config/config.h lib/sqlite/sqlite3.h
	cc -c src/config/config.c -o build/config.o $(sopt)

bin/tdj-localjudge: build/tdj-localjudge.o build/judger.o build/printf.o build/compare.o build/libconfig.a build/time.o
	cc build/tdj-localjudge.o build/judger.o build/printf.o build/compare.o build/libconfig.a build/time.o -o bin/tdj-localjudge $(opt)
build/tdj-localjudge.o: src/config/config.h src/judger/judger.h src/judger/compare.h src/judger/tdj-localjudge.c src/time/time.h
	cc -c src/judger/tdj-localjudge.c -o build/tdj-localjudge.o $(sopt)
build/judger.o: src/judger/judger.c src/judger/judger.h src/config/config.h
	cc -c src/judger/judger.c -o build/judger.o $(sopt)
build/compare.o: src/judger/compare.c src/judger/compare.h
	cc -c src/judger/compare.c -o build/compare.o $(sopt)

build/time.o: src/time/time.c src/time/time.h src/config/config.h
	cc -c src/time/time.c -o build/time.o $(sopt)

bin/tdj-server: build/tdj-server.o build/libconfig.a build/judger.o build/printf.o build/time.o build/compare.o build/libz.a
	cc build/tdj-server.o build/libconfig.a build/judger.o build/printf.o build/time.o build/compare.o build/libz.a -o bin/tdj-server $(opt)
build/tdj-server.o: src/server/tdj-server.c src/server/server_def.h src/judger/judger.h src/judger/compare.h src/time/time.h src/config/config.h src/z/zpipe.h lib/zlib/zlib.h
	cc -c src/server/tdj-server.c -o build/tdj-server.o $(sopt)

bin/tdj-broadcast: build/tdj-broadcast.o build/libconfig.a
	cc build/tdj-broadcast.o build/libconfig.a -o bin/tdj-broadcast $(opt)
build/tdj-broadcast.o: src/server/server_def.h src/config/config.h src/server/tdj-broadcast.c
	cc -c src/server/tdj-broadcast.c -o build/tdj-broadcast.o $(sopt)

bin/tdj-new: build/tdj-new.o
	cc build/tdj-new.o -o bin/tdj-new $(opt)
build/tdj-new.o: src/client/tdj-new.c
	cc -c src/client/tdj-new.c -o build/tdj-new.o $(sopt)

bin/tdj-push: build/tdj-push.o build/time.o build/libconfig.a build/libz.a
	cc build/tdj-push.o build/time.o build/libconfig.a build/libz.a -o bin/tdj-push $(opt)
build/tdj-push.o: src/client/tdj-push.c src/time/time.h src/config/config.h src/server/server_def.h src/judger/judger.h src/z/zpipe.h lib/zlib/zlib.h
	cc -c src/client/tdj-push.c -o build/tdj-push.o $(sopt)

bin/tdj-listener: build/tdj-listener.o build/libconfig.a
	c++ build/tdj-listener.o build/libconfig.a -o bin/tdj-listener $(opt)
build/tdj-listener.o: src/server/tdj-listener.cc src/server/server_def.h src/judger/judger.h src/config/config.h src/time/time.h
	c++ -c src/server/tdj-listener.cc -o build/tdj-listener.o $(sopt)

lib/sqlite/config.h: lib/sqlite/configure lib/sqlite/config.h.in
	cd lib/sqlite;./configure;cd ../..
build/sqlite3.o: lib/sqlite/sqlite3.c lib/sqlite/config.h
	cc -c lib/sqlite/sqlite3.c -o build/sqlite3.o -D_HAVE_SQLITE_CONFIG_H -Os
build/sql.o: src/config/sql.c lib/sqlite/sqlite3.h
	cc -c src/config/sql.c -o build/sql.o -DNO_KEEPER_LOG -DBIND_SIGNAL $(sopt)

build/libconfig.a: build/config.o build/sql.o build/sqlite3.o
	ar rc build/libconfig.a build/config.o build/sql.o build/sqlite3.o

lib/zlib/Makefile: lib/zlib/configure
	cd lib/zlib;./configure --static;cd ../..
lib/zlib/libz.a: lib/zlib/Makefile
	make -Clib/zlib libz.a
build/libz.a: lib/zlib/libz.a build/zpipe.o
	cp lib/zlib/libz.a build
	ar r build/libz.a build/zpipe.o
build/zpipe.o: src/z/zpipe.c lib/zlib/zlib.h
	cc -c src/z/zpipe.c -o build/zpipe.o -Ilib/zlib $(sopt)

build/printf.o: src/utility/printf.c src/utility/printf.h
	cc -c src/utility/printf.c -o build/printf.o $(sopt)

clean:
	-rm build/*
prepare:
	-mkdir build
	-mkdir bin
install:
	cp bin/* /usr/bin
reset:
	-rm -r bin
	-rm -r build
	-rm lib/sqlite/config.h
	make -Clib/zlib -fMakefile.in distclean
.PHONY: all clean prepare install reset
