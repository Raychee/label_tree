include ../Makefile.inc

LIBS = -L../lib -lsvmlight -lopencv_core -ljsgd
TARGET = main
OBJ = main.o label_tree_common.o LabelTree.o

ifeq ($(OS), LINUX)
LFLAGS += -Wl,-rpath,'$$ORIGIN/../lib'
else
CFLAGS += -mmacosx-version-min=10.5
LFLAGS += -Wl,-rpath,@executable_path/../lib
endif

ifeq ($(LD), clang)
LIBS += -lstdc++
endif


../bin/$(TARGET): $(OBJ) svm_light jsgd
	$(LD) $(LFLAGS) $(LIBS) $(OBJ) -o ../bin/$(TARGET)
	
main.o: main.cpp label_tree_common.o LabelTree.o
	$(CC) $(CFLAGS) -c $< -o $@
	
label_tree_common.o: label_tree_common.cpp \
					 label_tree_common.h StatTable.h List.h jsgd
	$(CC) $(CFLAGS) -c $< -o $@

LabelTree.o: LabelTree.cpp LabelTree.h
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: svm_light jsgd

jsgd:
	cd jsgd; make

svm_light:
	cd svm_light; make
