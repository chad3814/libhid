AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

dist_man1_MANS = libhid-detach-device.1

#TODO: must be determined automatically...
DB2MAN = /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/manpages/docbook.xsl
#DB2MAN = /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
XP = xsltproc -''-nonet
%.1: %.dbk
	$(XP) $(DB2MAN) $<

maintainerclean-local:
	rm -f ${wildcard *.[0-9]}
