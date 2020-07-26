CC=gcc
OBJS=server client

all: $(OBJS)

$(OBJS): %: %.c
	$(CC) -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJS) *~ core
