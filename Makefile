.PHONY: dogebc libopencm3

all: dogebc


dogebc: libopencm3
	$(MAKE) -C dogebc

libopencm3:
	$(MAKE) -C libopencm3

clean:
	$(MAKE) -C dogebc clean
	$(MAKE) -C libopencm3 clean
