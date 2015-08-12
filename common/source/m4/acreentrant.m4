dnl Some of these reentrant libc functions are accessible from both the standard
dnl libc library and the reentrant system libc library. However, their prototypes
dnl are concealed unless the MT compiler is used, or POSIX preprocessor symbols
dnl are defined. We'll just use the standard interfaces in a LITE environment,
dnl and copy things into the user buffers as neccessary.

AC_DEFUN([CHECK_REENTRANT_FUNCS],[

check_gethostbyaddr_r=yes
check_gethostbyname_r=yes


if test ${check_gethostbyaddr_r} = "yes" ; then

AC_CHECK_FUNCS(gethostbyaddr_r, [
	AC_MSG_CHECKING(number of arguments to gethostbyaddr_r)
	globus_gethostbyaddr_args=no
	AC_TRY_COMPILE([
#            include <sys/types.h>
#            include <netdb.h>],
        [
            char *address;
            int length;
            int type;
            struct hostent h;
            struct hostent_data hdata;
            int rc;

            rc = gethostbyaddr_r(address, length, type, &h, &hdata);
	], AC_DEFINE([GLOBUS_HAVE_GETHOSTBYADDR_R_5],[1],[Define to 1 if gethostbyaddr_r(address, length, type, hostent) exists]) globus_gethostbyaddr_args=5)
        if test $globus_gethostbyaddr_args = no; then
            AC_TRY_COMPILE([
#	        include <sys/types.h>
#               include <netdb.h>],
            [
                char *address;
                int length;
                int type;
                struct hostent h;
                char buffer[10];
                int buflen;
                int h_errnop;
                struct hostent *hp;

                hp = gethostbyaddr_r(address, length, type, &h,
                                     buffer, buflen, &h_errnop);
            ], AC_DEFINE([GLOBUS_HAVE_GETHOSTBYADDR_R_7],[1],[Define to 1 if gethostbyaddr_r(address, length, type, hostentp, buffer, buflen, herrnop) exists.]) globus_gethostbyaddr_args=7)
        fi

        if test $globus_gethostbyaddr_args = no; then
	   AC_TRY_COMPILE([
#              include <sys/types.h>
#              include <netdb.h>],
           [
               char *address;
               int length;
               int type;
	       int rc;
               struct hostent h;
               char buffer[10];
               int buflen;
               int h_errnop;
               struct hostent *hp;

               rc = gethostbyaddr_r(address, length, type, &h,
                                     buffer, buflen, &hp, &h_errnop); 

           ], AC_DEFINE(GLOBUS_HAVE_GETHOSTBYADDR_R_8,[1],[Define to 1 if gethostbyaddr_r(address, length, type, h, buffer, buflen, hp, herrnop) exists])
              globus_gethostbyaddr_args=8)
        fi
        AC_MSG_RESULT($globus_gethostbyaddr_args)
        ])
fi

AC_CHECK_FUNCS(gethostbyname)

AC_CHECK_FUNCS(gethostbyname_r, [
         AC_MSG_CHECKING(number of arguments to gethostbyname_r)
	 globus_gethostbyname_args=no
	 AC_TRY_COMPILE([
#            include <sys/types.h>
#            include <netdb.h>],
	 [
	     struct hostent *hp;
	     struct hostent h;
	     char *name;
	     char buffer[10];
	     int  h_errno;

	     hp = gethostbyname_r(name, &h, buffer, 10, &h_errno);
	 ], AC_DEFINE(GLOBUS_HAVE_GETHOSTBYNAME_R_5,[1],[Define to 1 if gethostbyname_r(name, h, buffer, len, herrno) exists]) globus_gethostbyname_args=5)
	 if test $globus_gethostbyname_args = no; then
	     AC_TRY_COMPILE([
#                include <sys/types.h>
#                include <netdb.h>],
             [
		 struct hostent h;
		 struct hostent_data hdata;
		 char *name;
		 int rc;

		 rc = gethostbyname_r(name, &h, &hdata);
	     ], AC_DEFINE(GLOBUS_HAVE_GETHOSTBYNAME_R_3, [1], [Define to 1 if gethostbyname_r(name, h, hdata) exists]) globus_gethostbyname_args=3)
	 fi
	 if test $globus_gethostbyname_args = no; then
	    AC_TRY_COMPILE([
#               include <sys/types.h>
#               include <netdb.h>
            ],
	    [
		struct hostent h;
		struct hostent *hp;
		char *name;
		char buf[10];
		int rc;
		int h_errno;

		rc = gethostbyname_r(name, &h, buf, 10, &hp, &h_errno);
            ], AC_DEFINE(GLOBUS_HAVE_GETHOSTBYNAME_R_6,[1],[Define to 1 if gethostbyname_r(name, h, buf, len, hp, herrno) exists]) globus_gethostbyname_args=6)
	 fi
	 AC_MSG_RESULT($globus_gethostbyname_args)
	 break;])


AC_CHECK_FUNCS(localtime_r)
AC_CHECK_FUNCS(gmtime_r)
AC_CHECK_FUNCS(ctime_r, 
    [
        AC_MSG_CHECKING(number of arguments to ctime_r)
        globus_ctime_args=no
        AC_TRY_COMPILE(
        [
#               include <time.h>
        ],
        [
            time_t clock;
            char buf[26];
            ctime_r(&clock, buf);
        ], AC_DEFINE([GLOBUS_HAVE_CTIME_R_2], [1], [Define to 1 if your system provides ctime_r(clock, buf)]) globus_ctime_args=2)
        if test $globus_ctime_args = no; then
            AC_TRY_COMPILE(
            [
#                   include <time.h>
            ],
            [
                time_t clock;
                char buf[26];
                ctime_r(&clock, buf, 26);
            ], AC_DEFINE([GLOBUS_HAVE_CTIME_R_3], [1], [Define to 1 if your system provides ctime_r(clock, buf, bufsize)]) globus_ctime_args=3)
        fi
        AC_MSG_RESULT($globus_ctime_args)
    ])

AC_MSG_CHECKING(if struct passwd contains pw_age)
globus_pw_age=no;
AC_TRY_COMPILE(
    [
#       include <pwd.h>
    ],
    [
        struct passwd pwd;
        char *x;
        x = pwd.pw_age;
    ], AC_DEFINE([GLOBUS_HAVE_PW_AGE],[1],[Define to 1 if struct passwd contains the pw_age member]) globus_pw_age=yes)
AC_MSG_RESULT($globus_pw_age)

AC_MSG_CHECKING(if struct passwd contains pw_comment)
globus_pw_comment=no;
AC_TRY_COMPILE(
    [
#       include <pwd.h>
    ],
    [
        struct passwd pwd;
        char *x;
        x = pwd.pw_comment;
    ], AC_DEFINE(GLOBUS_HAVE_PW_COMMENT,[1],[Define to 1 if struct passwd contains the pw_comment member]) globus_pw_comment=yes)
AC_MSG_RESULT($globus_pw_comment)


AC_CHECK_FUNCS(getpwnam)
AC_CHECK_FUNCS(getpwnam_r, 
    [
        AC_MSG_CHECKING(number of arguments to getpwnam_r)
        globus_getpwnam_args=no;

        AC_TRY_COMPILE(
        [
#           include <sys/types.h>
#           include <pwd.h>
        ],
        [
            char *name;
            struct passwd *pwptr;
            struct passwd pwd;
            char *buf;
            size_t buflen;
            int rc;
	
            rc = getpwnam_r(name, &pwd, buf, buflen, &pwptr);
        ],
        AC_DEFINE(GLOBUS_HAVE_GETPWNAM_R_5,[1],[Define to 1 if getpwname_r(name, pwd, buf, buflen, pwptr) exists]) globus_getpwnam_args=5)

        if test $globus_getpwnam_args = no; then
            AC_TRY_COMPILE(
            [
#           	include <sys/types.h>
#           	include <pwd.h>
            ],
            [
            	char *name;
            	struct passwd *pwptr;
            	struct passwd pwd;
            	char *buf;
            	int buflen;
	    
            	getpwnam_r(name, &pwd, buf, buflen);
            ],
            AC_DEFINE(GLOBUS_HAVE_GETPWNAM_R_4,[1],[Define to 1 if getpwnam_r(name, pwd, buf, buflen) exists]) globus_getpwnam_args=4)
        fi
        AC_MSG_RESULT($globus_getpwnam_args)
    ])

AC_CHECK_FUNCS(getpwuid)

AC_CHECK_FUNCS(getpwuid_r, 
    [
        AC_MSG_CHECKING(number of arguments to getpwuid_r)
        globus_getpwuid_args=no;

        AC_TRY_COMPILE(
        [
#           include <sys/types.h>
#           include <pwd.h>
        ],
        [
            uid_t uid;
            struct passwd *pwptr;
            struct passwd pwd;
            char *buf;
            size_t buflen;
            int rc;
	
            rc = getpwuid_r(uid, &pwd, buf, buflen, &pwptr);
        ],
        AC_DEFINE(GLOBUS_HAVE_GETPWUID_R_5,[1],[Define to 1 if getpwuid_r(uid, pwd, buf, buflen, pwptr) exists]) globus_getpwuid_args=5)

        if test $globus_getpwuid_args = no; then
            AC_TRY_COMPILE(
            [
#           	include <sys/types.h>
#           	include <pwd.h>
            ],
            [
	    	uid_t uid;
            	struct passwd *pwptr;
            	struct passwd pwd;
            	char *buf;
            	int buflen;
	    
            	getpwuid_r(uid, &pwd, buf, buflen);
            ],
            AC_DEFINE(GLOBUS_HAVE_GETPWUID_R_4,[1],[Define to 1 if getpwuid_r(uid, pwd, buf, buflen) exists]) globus_getpwuid_args=4)
        fi
        AC_MSG_RESULT($globus_getpwuid_args)
    ])

AC_CHECK_FUNCS(readdir_r, 
    [
        AC_MSG_CHECKING(number of arguments to readdir_r)
        globus_readdir_args=no;
        AC_TRY_COMPILE(
        [
#include <dirent.h>
        ],
        [
	    struct dirent dir, *dirp;
	    DIR *mydir;
	    int rc;
	    rc = readdir_r(mydir, &dir, &dirp);
        ],
        AC_DEFINE(GLOBUS_HAVE_READDIR_R_3,[1],[Define to 1 if readdir_r(dir, dirent, direntp) exists]) globus_readdir_args=3)


        if test $globus_readdir_args = no; then
            AC_TRY_COMPILE(
            [
#include <dirent.h>
            ],
            [
	    	struct dirent dir, *dirp;
	    	DIR *mydir;
	    	dirp = readdir_r(mydir, &dir);
            ],
	    AC_DEFINE(GLOBUS_HAVE_READDIR_R_2,[1],[Define to 1 if readdir_r(dir, dirp) exists]) globus_readdir_args=2)
        fi
        AC_MSG_RESULT($globus_readdir_args)
    ])

AC_MSG_CHECKING(if struct dirent contains d_off)
AC_TRY_COMPILE(
    [
#include <dirent.h>
    ],
    [
        struct dirent dir;
        ((int) dir.d_off) == 1;
    ],
    AC_MSG_RESULT([yes]); AC_DEFINE(GLOBUS_HAVE_DIRENT_OFF,[1],[Define to 1 if struct dirent has d_off member]),
    AC_MSG_RESULT([no]))

AC_MSG_CHECKING(if struct dirent contains d_offset)
AC_TRY_COMPILE(
    [
#include <dirent.h>
    ],
    [
struct dirent dir;
((int) dir.d_offset) == 1;
    ],
    AC_MSG_RESULT([yes]) ; AC_DEFINE(GLOBUS_HAVE_DIRENT_OFFSET,[1],[Define to 1 if struct dirent has d_offset member]), 
    AC_MSG_RESULT([no]))

AC_MSG_CHECKING(if struct dirent contains d_type)
AC_TRY_COMPILE(
    [
#include <dirent.h>
    ],
    [
struct dirent dir;
((int) dir.d_type) == 1;
    ],
    AC_MSG_RESULT([yes]) ; AC_DEFINE(GLOBUS_HAVE_DIRENT_TYPE,[1],[Define to 1 if struct dirent has d_type member]),
    AC_MSG_RESULT([no]))

AC_MSG_CHECKING(if struct dirent contains d_reclen)
AC_TRY_COMPILE(
    [
#include <dirent.h>
    ],
    [
struct dirent dir;
((int) dir.d_reclen) == 1;
    ],
    AC_MSG_RESULT([yes]) ; AC_DEFINE(GLOBUS_HAVE_DIRENT_RECLEN,1,[Define to 1 if struct dirent has d_reclen member]),
    AC_MSG_RESULT([no]))
])
