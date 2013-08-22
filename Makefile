include Makefile.inc

CFLAGS = -Wall -W -ggdb3 -pedantic -pipe
LFLAGS = -Wl,-rpath='$$ORIGIN/svm_light'
LIBS = -L./svm_light -lsvmlight -lopencv_core
TARGET = main
OBJ = main.o label_tree_common.o LabelTree.o

ifeq ($(LD) $(OS), clang LINUX)
LIBS += -lstdc++
endif

$(TARGET): $(OBJ) svm_light/libsvmlight.$(LIB_SUFFIX)
	$(LD) $(LFLAGS) -o $(TARGET) $(OBJ) $(LIBS)
	
main.o: main.cpp label_tree_common.o LabelTree.o
	$(CC) $(CFLAGS) -c -o $@ $<
	
label_tree_common.o: label_tree_common.cpp label_tree_common.h \
					 StatTable.h List.h
	$(CC) $(CFLAGS) -c -o $@ $<

LabelTree.o: LabelTree.cpp LabelTree.h
	$(CC) $(CFLAGS) -c -o $@ $<

svm_light/libsvmlight.$(LIB_SUFFIX):
	cd svm_light; make


.PHONY: clean cleanall

clean:
	rm -f *.o
	cd svm_light; rm -f *.o

cleanall:
	rm -rf $(TARGET) *.o *.dSYM
