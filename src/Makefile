include ../Makefile.inc

LIBS   = -L../lib -lopencv_core
OBJ    = labeltree_train.o SGD_SVM.o LabelTree.o
TARGET = labeltree_train

ifeq ($(OS), LINUX)
LFLAGS += -Wl,-rpath,'$$ORIGIN/../lib'
else
CFLAGS += -mmacosx-version-min=10.7
LFLAGS += -Wl,-rpath,@executable_path/../lib
endif

ifeq ($(LD), clang)
CFLAGS += -stdlib=libc++
LIBS   += -lc++
endif


../bin/$(TARGET): $(OBJ)
	$(LD) $(LFLAGS) $(LIBS) $(OBJ) -o ../bin/$(TARGET)
	
labeltree_train.o: labeltree_train.cpp SGD_SVM.o LabelTree.o
	$(CC) $(CFLAGS) -c $< -o $@
	
# label_tree_common.o: label_tree_common.cpp \
# 					 label_tree_common.h StatTable.h List.h jsgd
# 	$(CC) $(CFLAGS) -c $< -o $@
SGD_SVM.o: SGD_SVM.cpp SGD_SVM.h SGD.h my_typedefs.h
	$(CC) $(CFLAGS) -c $< -o $@

LabelTree.o: LabelTree.cpp LabelTree.h SGD.h my_typedefs.h
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: svm_light jsgd test

jsgd:
	cd jsgd; make

svm_light:
	cd svm_light; make

test:
# $(CC) $(CFLAGS) $(LFLAGS) $(LIBS) test.cpp -o ../bin/test
	$(CC) $(CFLAGS) -lc++ test.cpp LabelTree.cpp -o ../bin/test