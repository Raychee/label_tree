include Makefile.inc

CFLAGS = -Wall -W -ggdb3
LDFLAGS = -Wall -W -ggdb3
LIBS = -Lsvm_light -lsvmlight -lopencv_core -lstdc++
TARGET = main
OBJ = main.o label_tree_common.o


$(TARGET): $(OBJ) svm_light/libsvmlight.$(LIB_SUFFIX)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)
	
main.o: main.cpp label_tree_common.o LabelTree.h
	$(CC) $(CFLAGS) -c $<
	
label_tree_common.o: label_tree_common.cpp label_tree_common.h \
					 StatTable.h List.h
	$(CC) $(CFLAGS) -c $<

# FOR LINUX SYSTEMS
svm_light/libsvmlight.$(LIB_SUFFIX):
	cd svm_light; make


.PHONY: clean cleanall

clean:
	rm -f *.o
	cd svm_light; rm -f *.o

cleanall:
	rm -rf $(TARGET) *.o *.dSYM
