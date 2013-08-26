include ../Makefile.inc

CFLAGS = -O3 -fPIC
LFLAGS = 
OBJ = svm_learn_main.o svm_learn.o svm_common.o svm_hideo.o

ifeq ($(OS), LINUX)
LFLAGS += -shared
else
LFLAGS += -dynamiclib -Wl,-install_name,@rpath/libsvmlight.$(LIB_SUFFIX)
endif


libsvmlight.$(LIB_SUFFIX): $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) -o libsvmlight.$(LIB_SUFFIX)

svm_hideo.o: svm_hideo.c
	$(CC) -c $(CFLAGS) svm_hideo.c -o svm_hideo.o

svm_common.o: svm_common.c svm_common.h kernel.h
	$(CC) -c $(CFLAGS) svm_common.c -o svm_common.o

svm_learn.o: svm_learn.c svm_common.h
	$(CC) -c $(CFLAGS) svm_learn.c -o svm_learn.o

svm_learn_main.o: svm_learn_main.c svm_learn.h svm_common.h
	$(CC) -c $(CFLAGS) svm_learn_main.c -o svm_learn_main.o