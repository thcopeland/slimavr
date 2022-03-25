AS			= avra

src = $(wildcard *.S)
src_hex = $(patsubst %.S, %.hex, $(src))

all: $(src_hex)

%.hex: %.S
	$(AS) -o $@ -e /dev/null -d /dev/null $<

clean:
	rm -f $(src_hex)
