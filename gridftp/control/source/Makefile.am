ACLOCAL_AMFLAGS=-I m4
pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = . test

include_HEADERS = globus_ftp_control.h
lib_LTLIBRARIES = libglobus_ftp_control.la
doc_DATA = GLOBUS_LICENSE
pkgconfig_DATA = globus-ftp-control.pc

libglobus_ftp_control_la_CPPFLAGS = $(PACKAGE_DEP_CFLAGS)
libglobus_ftp_control_la_LIBADD = $(PACKAGE_DEP_LIBS)
libglobus_ftp_control_la_LDFLAGS = \
        -version-info $(MAJOR_VERSION):$(MINOR_VERSION):$(AGE_VERSION) \
        -no-undefined
libglobus_ftp_control_la_SOURCES = \
	jansson_dump.c \	
	jansson_error.c \
	jansson_hashtable_seed.c \
	jansson_hashtable.c \
	jansson_load.c \
	jansson_memory.c \
	jansson_pack_unpack.c \
	jansson_strbuffer.c \
	jansson_strconv.c \
	jansson_utf.c \
	jansson_value.c \
	globus_ftp_control.c \
	globus_ftp_control_client.c \
	globus_ftp_control_data.c \
	globus_ftp_control_layout.c \
	globus_ftp_control_server.c \
	jansson_config.h \
	jansson_hashtable.h \
	jansson_lookup3.h \
	jansson_private.h \
	jansson_strbuffer.h \
	jansson_utf.h \
	jansson.h \
	globus_ftp_control.h \
	globus_i_ftp_control.h

EXTRA_DIST = \
	dirt.sh \
	globus_gridftp_protocol_extensions.dox \
	$(doc_DATA)

DOC_STAMPS =
if ENABLE_DOXYGEN
DOC_STAMPS += doc-stamp

doc-stamp: Doxyfile $(libglobus_common_la_SOURCES) globus_gridftp_protocol_extensions.dox
	rm -rf doc
	$(DOXYGEN)
	touch $@

clean-local:
	rm -rf doc doc-stamp
endif

all-local: $(DOC_STAMPS)

install-data-local: $(DOC_STAMPS)
	if test -d doc/man/man3; then \
		install -d -m 755 $(DESTDIR)$(mandir)/man3; \
	        $(INSTALL) -m 644 doc/man/man3/[Gg][Ll][Oo][Bb][Uu][Ss]*.3 $(DESTDIR)$(mandir)/man3 ; \
        fi
	if test -d doc/html; then \
		for dir in `(cd doc; find html -type d)`; do \
			install -d -m 755 $(DESTDIR)$(docdir)/$$dir; \
		done ; \
		for f in `(cd doc; find html -type f)`; do \
			$(INSTALL) -m 644 doc/$$f $(DESTDIR)$(docdir)/$$f ; \
		done ; \
	fi

distuninstallcheck:
	@:
