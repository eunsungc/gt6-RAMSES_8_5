# gt6-RAMSES_8_5
version 1439213997

# Install procedure
1) ./configure
2) modify aclocal.m4: replace the version of automake as appropriate.
   e.g., 1.14.1 --> 1.14
3) modify Makefile: For some reasons, make tries to update aclocal.m4. So comment out several lines starting with 'ACLOCAL_M4:'.
4) make; make install
