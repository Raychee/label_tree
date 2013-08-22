OS ?= LINUX

ifeq ($(OS), LINUX)
LIB_SUFFIX = so
else	# $(OS) = MAC
LIB_SUFFIX = dylib
endif

CC = clang
LD = clang
