# gt6-RAMSES_8_5
version 1439213997

# Install procedure
0. `git clone https://github.com/eunsungc/gt6-RAMSES_8_5`
0. `cd gt6-RAMSES_8_5`
0. `./build.sh`: The scrip will do the following. At step 3, we have to modify Makefile manually.
* 1) ./configure
* 2) modify aclocal.m4: replace the version of automake as appropriate.  e.g., 1.14.1 --> 1.14
* 3) modify Makefile: For some reasons, make tries to update aclocal.m4. So comment out several lines as follows.
```
#$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
#   @for dep in $?; do \
#     case '$(am__configure_deps)' in \
#       *$$dep*) \
#         echo ' cd $(srcdir) && $(AUTOMAKE) --foreign'; \
#         $(am__cd) $(srcdir) && $(AUTOMAKE) --foreign \
#       && exit 0; \
#         exit 1;; \
#     esac; \
#   done; \
#   echo ' cd $(top_srcdir) && $(AUTOMAKE) --foreign Makefile'; \
#   $(am__cd) $(top_srcdir) && \
#     $(AUTOMAKE) --foreign Makefile
.PRECIOUS: Makefile
#Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
#    @case '$?' in \
#      *config.status*) \
#        echo ' $(SHELL) ./config.status'; \
#        $(SHELL) ./config.status;; \
#      *) \
#        echo ' cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe)'; \
#        cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe);; \
#    esac;

#$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
#    $(SHELL) ./config.status --recheck

#$(top_srcdir)/configure:  $(am__configure_deps)
#   $(am__cd) $(srcdir) && $(AUTOCONF)
#$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
#   $(am__cd) $(srcdir) && $(ACLOCAL) $(ACLOCAL_AMFLAGS)
#$(am__aclocal_m4_deps):

ltdlconfig.h: stamp-h1
        @test -f $@ || rm -f stamp-h1
        @test -f $@ || $(MAKE) $(AM_MAKEFLAGS) stamp-h1

stamp-h1: $(srcdir)/ltdlconfig.h.in $(top_builddir)/config.status
        @rm -f stamp-h1
        cd $(top_builddir) && $(SHELL) ./config.status ltdlconfig.h
#$(srcdir)/ltdlconfig.h.in:  $(am__configure_deps) 
#       ($(am__cd) $(top_srcdir) && $(AUTOHEADER))
#       rm -f stamp-h1
#       touch $@

```
* 4) find . -type f -name aclocal.m4 -exec touch {} \;
* 5) find . -type f -name Makefile.in -exec touch {} \;
* 6) make; make install
0.
