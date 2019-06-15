BINDIR?=.

CFLAGS+=-std=c89 -Wall -Wextra -Werror -Wno-unused-parameter $(shell pkg-config --cflags prs)
LDFLAGS+=$(shell pkg-config --libs prs)

SOURCES=$(shell find . -maxdepth 1 -iname '*.c')
OBJECTS=$(SOURCES:%.c=%.c.o)
TARGET=creatproj

.PHONY: all clean
all: $(TARGET)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)

clean:
	rm -f *~ *.log $(OBJECTS) $(BINDIR)/$(TARGET)
