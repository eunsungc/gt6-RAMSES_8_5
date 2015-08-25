# gt6-RAMSES_8_5
version 1439213997

# Install procedure
1. `git clone https://github.com/eunsungc/gt6-RAMSES_8_5`
2. `cd gt6-RAMSES_8_5`
3. `./build.sh`: The script will do the following. At step iii, we have to modify Makefile manually. automake/autoconf 1.14 version is required.
  1. ./configure
  2. ~~modify aclocal.m4: replace the version of automake as appropriate.  e.g., 1.14.1 --> 1.14~~
  3. modify Makefile: For some reasons, make tries to update aclocal.m4. So comment out several lines as follows.
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
  4. find . -type f -name aclocal.m4 -exec touch {} \;
  5. find . -type f -name Makefile.in -exec touch {} \;
  6. make; make install

# Log files
* Additional json files are generated corresponding to the existing log files. For example, if the gridftp.conf looks like as follows, `gridftp-ramses.log.json` and `gridftp-transfer-ramses.log.json` will be created.

```
log_single /home/esjung/gridftp-ramses.log

log_transfer /home/esjung/gridftp-transfer-ramses.log
```

# CHANGES
* Multiline JSON object -> single line JSON object.
* mpstat information has only average CPU stats; reduced log size.
* iostat information tries to capture the device the file belongs to; reduced log size.
  * First, identify the device; currently, support for nfs devices, but no support for gpfs file system devices.
  * Second, try `iostat/nfsiostat 1 2` to compute throughput.
* Add more fields such as starttime, endtime, etc.
* Separate files for json logging.

# Troubleshooting
* If log messages are broken, try `globus-gridftp-server -log-module stdio:buffer=32768:interval=0`
