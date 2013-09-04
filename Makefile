.PHONY: main test clean cleanall

main:
	cd src; make

test:
	cd src; make test

clean:
	rm -f src/*/*.o src/*.o

cleanall:
	rm -f src/*/*.o src/*.o src/*.dSYM
	rm -f bin/*
