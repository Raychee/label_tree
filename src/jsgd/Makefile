include ../../Makefile.inc

CC = gcc
LD = gcc
CFLAGS += -fPIC -fopenmp -Dinteger=long
LFLAGS += -fopenmp
OBJ = jsgd.o x_matrix.o
LIBS = -latlas

ifeq ($(OS), LINUX)
LFLAGS += -shared
else
LFLAGS += -dynamiclib -Wl,-install_name,@rpath/libjsgd.$(LIB_SUFFIX)
endif

../../lib/libjsgd.$(LIB_SUFFIX): $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) -o ../../lib/libjsgd.$(LIB_SUFFIX) $(LIBS)

jsgd.o: jsgd.c jsgd.h x_matrix.o
	$(CC) $(CFLAGS) -c $< -o $@

x_matrix.o: x_matrix.c x_matrix.h
	$(CC) $(CFLAGS) -c $< -o $@