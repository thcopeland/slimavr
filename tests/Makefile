SRC				= ../src
CFLAGS			= -O2 -flto -I$(SRC)
LDLIBS			= ../libslimavr.a

tests = $(wildcard *.c)
tests_ex = $(patsubst %.c, %.out, $(tests))

all: asm $(tests_ex)

asm: .FORCE
	make -C asm

%.out: %.c $(LDLIBS)
	gcc $(CFLAGS) $^ $(LDLIBS) -o $@

run: all
	for f in $(tests_ex); do ./$$f; done

clean:
	rm -f $(tests_ex)

.FORCE:
