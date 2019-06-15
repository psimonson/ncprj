# Main project Makefile by Philip R. Simonson
######################################################

SRCDIR=$(shell pwd)
BINDIR=$(SRCDIR)/bin
BASENAME=$(shell echo $$(basename $(SRCDIR)))
VERSION=0.1

DESTDIR?=
PREFIX?=usr/local

DEBUG?=0	# simple debug flag 1=true,0=false

ifeq ($(DEBUG),1)
	CFLAGS+=-g
endif

.PHONY: all clean distclean dist
all: configure creatproj

configure:
	@echo Building configure...
	@mkdir -p $(BINDIR)
	@cd confsrc && BINDIR="${BINDIR}" CFLAGS="${CFLAGS}" \
	LDFLAGS="${LDFLAGS}" $(MAKE) all && echo Built configure. || die

creatproj:
	@echo Building creatproj...
	@mkdir -p $(BINDIR)
	@cd creatsrc && BINDIR="${BINDIR}" CFLAGS="${CFLAGS}" \
	LDFLAGS="${LDFLAGS}" $(MAKE) all && echo Built creatproj. || die

clean:
	@echo Cleaning build files...
	@cd confsrc && BINDIR="${BINDIR}" $(MAKE) clean && \
	echo Cleaned configure. || die
	@cd creatsrc && BINDIR="${BINDIR}" $(MAKE) clean && \
	echo Cleaned creatproj. || die

distclean: clean
	rm -rf *~ *.log $(BINDIR)

dist:
	@echo Compressing distribution $(BASENAME)-$(VERSION).tgz...
	@cd .. && tar cf - ./$(BASENAME) | gzip -9 > \
	$(BASENAME)-$(VERSION).tgz && echo Compression successful. \
	|| die

install:
	@echo Installing binaries...
	@mkdir -p $(DESTDIR)/$(PREFIX)/bin
	@install $(BINDIR)/* $(DESTDIR)/$(PREFIX)/bin && \
	echo Binaries installed. || die
	@echo Installing manpages...
	@mkdir -p $(DESTDIR)/$(PREFIX)/share/man/man1
	@install man/* $(DESTDIR)/$(PREFIX)/share/man/man1 && \
	bzip2 $(DESTDIR)/$(PREFIX)/share/man/man1/confproj.1 && \
	bzip2 $(DESTDIR)/$(PREFIX)/share/man/man1/creatproj.1 && \
	echo Manpages installed. || die
	@echo Installation finished.

uninstall:
	@echo Uninstalling binaries...
	@rm -f $(DESTDIR)/$(PREFIX)/bin/confproj \
	$(DESTDIR)/$(PREFIX)/bin/creatproj && \
	echo Binaries uninstalled. || die
	@echo Uinstalling manpages...
	@rm -f $(DESTDIR)/$(PREFIX)/share/man/man1/confproj.1.bz2 \
	$(DESTDIR)/$(PREFIX)/share/man/man1/creatproj.1.bz2 && \
	echo Manpages uninstalled. || die
	@echo Uninstall finished.

