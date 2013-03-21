CC        = gcc
CFLAGS    = -Wall -O2
LIBS      = -lX11 -lXtst
PREFIX    = /usr/local
BINPREFIX = $(PREFIX)/bin

all:
	$(CC) -o keylock keylock.c $(CFLAGS) $(LIBS)

install:
	@echo "Installing keylock in $(DESTDIR)$(BINPREFIX)"
	@install -D -m 755 keylock $(DESTDIR)$(BINPREFIX)/keylock

uninstall:
	@echo "Removing keylock from $"DESTDIR)$(BINPREFIX)"
	@rm -f $(DESTDIR)$(BINPREFIX)/{keylock}

