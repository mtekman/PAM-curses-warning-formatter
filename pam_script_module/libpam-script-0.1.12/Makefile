VERSION		= 0.1.3
CC		= gcc
DEBUG		= -g
CFLAGS		= -Wall -pedantic -fPIC
SHARED		= -shared 
DESTDIR 	= /
prefix  	= /usr
bindir  	= $(prefix)/bin
plibdir 	= lib/security
sysconfdir	= /etc

all: pam_script.so

pam_script.so: pam_script.c
	$(CC) $(CFLAGS) $(SHARED) -o pam_script.so pam_script.c

install: all
	install -d $(DESTDIR)/$(plibdir)
	install pam_script.so $(DESTDIR)/$(plibdir)

tar: clean
	cd ..; tar czf pam_script-$(VERSION).tar.gz pam_script-$(VERSION)

rpm: tar
	cp ../pam_script-$(VERSION).tar.gz /usr/src/redhat/SOURCES/
	rpm -ba pam_script.spec

clean:
	rm -f pam_script.so
