include ../Makefile.inc

CFLAGS = -O3 -fPIC
LFLAGS = -O3
OBJ = svm_learn_main.o svm_learn.o svm_common.o svm_hideo.o


libsvmlight.$(LIB_SUFFIX): $(OBJ)
ifeq ($(OS), LINUX)
	$(LD) $(LFLAGS) -shared $(OBJ) -o libsvmlight.so 
else
	$(LD) $(LFLAGS) -dynamiclib $(OBJ) -o libsvmlight.dylib
endif

svm_hideo.o: svm_hideo.c
	$(CC) -c $(CFLAGS) svm_hideo.c -o svm_hideo.o

svm_common.o: svm_common.c svm_common.h kernel.h
	$(CC) -c $(CFLAGS) svm_common.c -o svm_common.o

svm_learn.o: svm_learn.c svm_common.h
	$(CC) -c $(CFLAGS) svm_learn.c -o svm_learn.o

svm_learn_main.o: svm_learn_main.c svm_learn.h svm_common.h
	$(CC) -c $(CFLAGS) svm_learn_main.c -o svm_learn_main.o