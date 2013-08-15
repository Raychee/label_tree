CC = g++
LD = g++
CFLAGS = -Wall -W -ggdb3
LDFLAGS = -Wall -W -ggdb3
LIBS = -Lsvm_light -lsvmlight -lopencv_core
TARGET = main
OBJ = main.o label_tree_common.o



$(TARGET): $(OBJ) svm_light/libsvmlight.dylib
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)
	
main.o: main.cpp label_tree_common.o
	$(CC) $(CFLAGS) -c $<
	
label_tree_common.o: label_tree_common.cpp label_tree_common.h \
					 StatTable.h LabelTree.h List.h
	$(CC) $(CFLAGS) -c $<


# gurantee the access to the library of svm_light
# FOR MAC OS ONLY!
svm_light/libsvmlight.dylib: 
	cd svm_light; make -f Makefile_MacOS

# FOR LINUX SYSTEMS
#svm_light/libsvmlight.so:
#	cd svm_light; make libsvmlight_hideo


.PHONY: clean cleanall

clean:
	rm -f *.o
	cd svm_light; rm -f *.o

cleanall:
	rm -rf $(TARGET) *.o *.dSYM