SRC				= ../src
OBJ 			= ../obj
CFLAGS			= -O2 -flto -I$(SRC)
LDLIBS			= ../libslimavr.a

tests = $(wildcard *.c)
tests_ex = $(patsubst %.c, %.out, $(tests))

all: $(tests_ex)

clean:
	rm -f $(tests_ex)
