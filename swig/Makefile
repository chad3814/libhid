CPPFLAGS = -I/usr/include/python2.3/ -I../include -DHID_INTERNAL
CFLAGS = -fpic
LDFLAGS = -shared -lusb -lhid

SWIGFILES = $(wildcard *.i)
WRAPPERS = $(foreach i,$(SWIGFILES),$(patsubst %.i,%_wrap.o,$(i)))

hid.so: $(WRAPPERS)
	$(LD) $(LDFLAGS) $^ -o $@

%_wrap.c: %.i
	swig -python $<

startup.py: $(LIBSOS)
	echo "#!/usr/bin/python" > $@
	echo "" >> $@
	for i in *.so; do \
		echo "from $$i import *" | sed -e 's,_\(.*\)\.so,\1,'; \
	done >> $@

run: hid.so startup.py
	env PYTHONSTARTUP=./startup.py python

.PHONY: clean
clean:
	rm -f $(wildcard *.py) $(wildcard *.pyc)
	rm -f $(wildcard *.c)
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.so)
