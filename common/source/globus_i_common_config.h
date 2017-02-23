/* globus_i_common_config.h.  Generated from globus_i_common_config.h.in by configure.  */
/* globus_i_common_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef CLOSEDIR_VOID */

/* Same as `HAVE_DIRENT_H', don't depend on me. */
#define DIRENT 1

/* Define to 1 if your system provides ctime_r(clock, buf) */
#define GLOBUS_HAVE_CTIME_R_2 1

/* Define to 1 if your system provides ctime_r(clock, buf, bufsize) */
/* #undef GLOBUS_HAVE_CTIME_R_3 */

/* Define to 1 if struct dirent has d_off member */
#define GLOBUS_HAVE_DIRENT_OFF 1

/* Define to 1 if struct dirent has d_offset member */
/* #undef GLOBUS_HAVE_DIRENT_OFFSET */

/* Define to 1 if struct dirent has d_reclen member */
#define GLOBUS_HAVE_DIRENT_RECLEN 1

/* Define to 1 if struct dirent has d_type member */
#define GLOBUS_HAVE_DIRENT_TYPE 1

/* Define to 1 if gethostbyaddr_r(address, length, type, hostent) exists */
/* #undef GLOBUS_HAVE_GETHOSTBYADDR_R_5 */

/* Define to 1 if gethostbyaddr_r(address, length, type, hostentp, buffer,
   buflen, herrnop) exists. */
/* #undef GLOBUS_HAVE_GETHOSTBYADDR_R_7 */

/* Define to 1 if gethostbyaddr_r(address, length, type, h, buffer, buflen,
   hp, herrnop) exists */
#define GLOBUS_HAVE_GETHOSTBYADDR_R_8 1

/* Define to 1 if gethostbyname_r(name, h, hdata) exists */
/* #undef GLOBUS_HAVE_GETHOSTBYNAME_R_3 */

/* Define to 1 if gethostbyname_r(name, h, buffer, len, herrno) exists */
/* #undef GLOBUS_HAVE_GETHOSTBYNAME_R_5 */

/* Define to 1 if gethostbyname_r(name, h, buf, len, hp, herrno) exists */
#define GLOBUS_HAVE_GETHOSTBYNAME_R_6 1

/* Define to 1 if getpwnam_r(name, pwd, buf, buflen) exists */
/* #undef GLOBUS_HAVE_GETPWNAM_R_4 */

/* Define to 1 if getpwname_r(name, pwd, buf, buflen, pwptr) exists */
#define GLOBUS_HAVE_GETPWNAM_R_5 1

/* Define to 1 if getpwuid_r(uid, pwd, buf, buflen) exists */
/* #undef GLOBUS_HAVE_GETPWUID_R_4 */

/* Define to 1 if getpwuid_r(uid, pwd, buf, buflen, pwptr) exists */
#define GLOBUS_HAVE_GETPWUID_R_5 1

/* Define to 1 if struct passwd contains the pw_age member */
/* #undef GLOBUS_HAVE_PW_AGE */

/* Define to 1 if struct passwd contains the pw_comment member */
/* #undef GLOBUS_HAVE_PW_COMMENT */

/* Define to 1 if readdir_r(dir, dirp) exists */
/* #undef GLOBUS_HAVE_READDIR_R_2 */

/* Define to 1 if readdir_r(dir, dirent, direntp) exists */
#define GLOBUS_HAVE_READDIR_R_3 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the `ctime_r' function. */
#define HAVE_CTIME_R 1

/* Define to 1 if you have the declaration of `cygwin_conv_path', and to 0 if
   you don't. */
/* #undef HAVE_DECL_CYGWIN_CONV_PATH */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define if you have the GNU dld library. */
/* #undef HAVE_DLD */

/* Define to 1 if you have the `dlerror' function. */
#define HAVE_DLERROR 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the _dyld_func_lookup function. */
/* #undef HAVE_DYLD */

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `geteuid' function. */
#define HAVE_GETEUID 1

/* Define to 1 if you have the `gethostbyaddr_r' function. */
#define HAVE_GETHOSTBYADDR_R 1

/* Define to 1 if you have the `gethostbyname' function. */
#define HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the `gethostbyname_r' function. */
#define HAVE_GETHOSTBYNAME_R 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the `getpwnam' function. */
#define HAVE_GETPWNAM 1

/* Define to 1 if you have the `getpwnam_r' function. */
#define HAVE_GETPWNAM_R 1

/* Define to 1 if you have the `getpwuid' function. */
#define HAVE_GETPWUID 1

/* Define to 1 if you have the `getpwuid_r' function. */
#define HAVE_GETPWUID_R 1

/* Define to 1 if you have the `gmtime_r' function. */
#define HAVE_GMTIME_R 1

/* Define to 1 if you have the <ifaddrs.h> header file. */
#define HAVE_IFADDRS_H 1

/* Define to 1 if you have the <if/arp.h> header file. */
/* #undef HAVE_IF_ARP_H */

/* Define to 1 if you have the <if/dl.h> header file. */
/* #undef HAVE_IF_DL_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the libdl library or equivalent. */
#define HAVE_LIBDL 1

/* Define if libdlloader will be built on this platform */
#define HAVE_LIBDLLOADER 1

/* Define to 1 if you have the `ltdl' library (-lltdl). */
/* #undef HAVE_LIBLTDL */

/* Define to 1 if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if you have the <ltdl.h> header file. */
#define HAVE_LTDL_H 1

/* Define to 1 if you have the `lt_dlmutex_register' function. */
/* #undef HAVE_LT_DLMUTEX_REGISTER */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <net/if_arp.h> header file. */
#define HAVE_NET_IF_ARP_H 1

/* Define to 1 if you have the <net/if_dl.h> header file. */
/* #undef HAVE_NET_IF_DL_H */

/* Define to 1 if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define to 1 if you have the `readdir_r' function. */
#define HAVE_READDIR_R 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the `seekdir' function. */
#define HAVE_SEEKDIR 1

/* Define if you have the shl_load function. */
/* #undef HAVE_SHL_LOAD */

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigprocmask' function. */
#define HAVE_SIGPROCMASK 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysctl.h> header file. */
#define HAVE_SYS_SYSCTL_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `telldir' function. */
#define HAVE_TELLDIR 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Same as `HAVE_NDIR_H', don't depend on me. */
#define NDIR 1

/* Name of package */
#define PACKAGE "globus_common"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://globus.atlassian.net/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "globus_common"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "globus_common 15.31"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "globus_common"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "15.31"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Same as `HAVE_SYS_DIR_H', don't depend on me. */
#define SYSDIR 1

/* Same as `HAVE_SYS_NDIR_H', don't depend on me. */
#define SYSNDIR 1

/* Use backward-compatibility symbol labels */
/* #undef USE_SYMBOL_LABELS */

/* Version number of package */
#define VERSION "15.31"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
