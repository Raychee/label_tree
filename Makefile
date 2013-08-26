include Makefile.inc

CFLAGS = -Wall -W -ggdb3 -pedantic -pipe
LFLAGS = 
LIBS = -L./svm_light -lsvmlight -lopencv_core
TARGET = main
OBJ = main.o label_tree_common.o LabelTree.o

ifeq ($(OS), LINUX)
LFLAGS += -Wl,-rpath,'$$ORIGIN/svm_light'
else
CFLAGS += -mmacosx-version-min=10.5
LFLAGS += -Wl,-rpath,@executable_path/svm_light
endif

ifeq ($(LD), clang)
LIBS += -lstdc++
endif

$(TARGET): $(OBJ) svm_light/libsvmlight.$(LIB_SUFFIX)
	$(LD) $(LFLAGS) $(LIBS) $(OBJ) -o $(TARGET)
	
main.o: main.cpp label_tree_common.o LabelTree.o
	$(CC) $(CFLAGS) -c $< -o $@
	
label_tree_common.o: label_tree_common.cpp \
					 label_tree_common.h StatTable.h List.h
	$(CC) $(CFLAGS) -c $< -o $@

LabelTree.o: LabelTree.cpp LabelTree.h
	$(CC) $(CFLAGS) -c $< -o $@

svm_light/libsvmlight.$(LIB_SUFFIX):
	cd svm_light; make


.PHONY: clean cleanall

clean:
	rm -f *.o
	cd svm_light; rm -f *.o

cleanall:
	rm -rf $(TARGET) *.o *.dSYM
