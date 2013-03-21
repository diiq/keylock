CC        = gcc
CFLAGS    = -Wall -O2
LIBS      = -lX11 -lXtst
PREFIX    = /usr/local
BINPREFIX = $(PREFIX)/bin

SRC = keylock.c
all: keylock

keylock: $(SRC) Makefile
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LIBS)
install: all
	@echo "Installing keylock in $(DESTDIR)$(BINPREFIX)"
	@install -D -m 755 keylock $(DESTDIR)$(BINPREFIX)/keylock

uninstall:
	@echo "Removing keylock from $"DESTDIR)$(BINPREFIX)"
	@rm -f $(DESTDIR)$(BINPREFIX)/{keylock}

