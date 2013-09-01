.PHONY: main clean cleanall

main:
	cd src; make

clean:
	rm -rf *.o

cleanall:
	rm -f src/*/*.o src/*.o src/*.dSYM
	rm -f bin/*
