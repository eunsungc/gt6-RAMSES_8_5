/* $OpenBSD: pathnames.h,v 1.23 2013/04/05 00:31:49 djm Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/*
 * Some of these pathnames are generated at run-time to meet GPT binary
 * relocatability requirements.  They are initialized by init_pathnames()
 * in pathnames.c.
 */
void init_pathnames();

/*
 * System-wide file containing host keys of known hosts.  This file should be
 * world-readable.
 */
extern char *_PATH_SSH_SYSTEM_HOSTFILE;
/* backward compat for protocol 2 */
extern char *_PATH_SSH_SYSTEM_HOSTFILE2;

/*
 * Of these, ssh_host_key must be readable only by root, whereas ssh_config
 * should be world-readable.
 */
extern char *_PATH_SERVER_CONFIG_FILE;
extern char *_PATH_HOST_CONFIG_FILE;
extern char *_PATH_HOST_KEY_FILE;
extern char *_PATH_HOST_DSA_KEY_FILE;
extern char *_PATH_HOST_ECDSA_KEY_FILE;
extern char *_PATH_HOST_RSA_KEY_FILE;
extern char *_PATH_DH_MODULI;
/* Backwards compatibility */
extern char *_PATH_DH_PRIMES;

extern char *_PATH_SSH_PROGRAM;

/*
 * The process id of the daemon listening for connections is saved here to
 * make it easier to kill the correct daemon when necessary.
 */
extern char *_PATH_SSH_DAEMON_PID_FILE;

/*
 * The directory in user's home directory in which the files reside. The
 * directory should be world-readable (though not all files are).
 */
#define _PATH_SSH_USER_DIR		".ssh"

/*
 * Per-user file containing host keys of known hosts.  This file need not be
 * readable by anyone except the user him/herself, though this does not
 * contain anything particularly secret.
 */
#define _PATH_SSH_USER_HOSTFILE		"~/" _PATH_SSH_USER_DIR "/known_hosts"
/* backward compat for protocol 2 */
#define _PATH_SSH_USER_HOSTFILE2	"~/" _PATH_SSH_USER_DIR "/known_hosts2"

/*
 * Name of the default file containing client-side authentication key. This
 * file should only be readable by the user him/herself.
 */
#define _PATH_SSH_CLIENT_IDENTITY	_PATH_SSH_USER_DIR "/identity"
#define _PATH_SSH_CLIENT_ID_DSA		_PATH_SSH_USER_DIR "/id_dsa"
#define _PATH_SSH_CLIENT_ID_ECDSA	_PATH_SSH_USER_DIR "/id_ecdsa"
#define _PATH_SSH_CLIENT_ID_RSA		_PATH_SSH_USER_DIR "/id_rsa"

/*
 * Configuration file in user's home directory.  This file need not be
 * readable by anyone but the user him/herself, but does not contain anything
 * particularly secret.  If the user's home directory resides on an NFS
 * volume where root is mapped to nobody, this may need to be world-readable.
 */
#define _PATH_SSH_USER_CONFFILE		_PATH_SSH_USER_DIR "/config"

/*
 * File containing a list of those rsa keys that permit logging in as this
 * user.  This file need not be readable by anyone but the user him/herself,
 * but does not contain anything particularly secret.  If the user's home
 * directory resides on an NFS volume where root is mapped to nobody, this
 * may need to be world-readable.  (This file is read by the daemon which is
 * running as root.)
 */
#define _PATH_SSH_USER_PERMITTED_KEYS	_PATH_SSH_USER_DIR "/authorized_keys"

/* backward compat for protocol v2 */
#define _PATH_SSH_USER_PERMITTED_KEYS2	_PATH_SSH_USER_DIR "/authorized_keys2"

/*
 * Per-user and system-wide ssh "rc" files.  These files are executed with
 * /bin/sh before starting the shell or command if they exist.  They will be
 * passed "proto cookie" as arguments if X11 forwarding with spoofing is in
 * use.  xauth will be run if neither of these exists.
 */
#define _PATH_SSH_USER_RC		_PATH_SSH_USER_DIR "/rc"
extern char *_PATH_SSH_SYSTEM_RC;

/*
 * Ssh-only version of /etc/hosts.equiv.  Additionally, the daemon may use
 * ~/.rhosts and /etc/hosts.equiv if rhosts authentication is enabled.
 */
extern char *_PATH_SSH_HOSTS_EQUIV;
#define _PATH_RHOSTS_EQUIV		"/etc/hosts.equiv"

/*
 * Default location of askpass
 */
#ifndef _PATH_SSH_ASKPASS_DEFAULT
#define _PATH_SSH_ASKPASS_DEFAULT	"/usr/X11R6/bin/ssh-askpass"
#endif

/* Location of ssh-keysign for hostbased authentication */
extern char *_PATH_SSH_KEY_SIGN;

/* Location of ssh-pkcs11-helper to support keys in tokens */
#ifndef _PATH_SSH_PKCS11_HELPER
#define _PATH_SSH_PKCS11_HELPER		"/usr/libexec/ssh-pkcs11-helper"
#endif

/* xauth for X11 forwarding */
#ifndef _PATH_XAUTH
#ifdef WIN32_FIXME
#define _PATH_XAUTH			"./auth.exe"
#else
#define _PATH_XAUTH			"/usr/X11R6/bin/xauth"
#endif
#endif

/* UNIX domain socket for X11 server; displaynum will replace %u */
#ifndef _PATH_UNIX_X
#define _PATH_UNIX_X "/tmp/.X11-unix/X%u"
#endif

/* for scp */
#ifndef _PATH_CP
#define _PATH_CP			"cp"
#endif

/* for sftp */
extern char *_PATH_SFTP_SERVER;

/* chroot directory for unprivileged user when UsePrivilegeSeparation=yes */
#ifndef _PATH_PRIVSEP_CHROOT_DIR
#define _PATH_PRIVSEP_CHROOT_DIR	"/var/empty"
#endif

/* for passwd change */
#ifndef _PATH_PASSWD_PROG
#define _PATH_PASSWD_PROG             "/usr/bin/passwd"
#endif

#ifndef _PATH_LS
#define _PATH_LS			"ls"
#endif

/* path to login program */
#ifndef LOGIN_PROGRAM
# ifdef LOGIN_PROGRAM_FALLBACK
#  define LOGIN_PROGRAM         LOGIN_PROGRAM_FALLBACK
# else
#  define LOGIN_PROGRAM         "/usr/bin/login"
# endif
#endif /* LOGIN_PROGRAM */

/* Askpass program define */
#ifndef ASKPASS_PROGRAM
#define ASKPASS_PROGRAM         "/usr/lib/ssh/ssh-askpass"
#endif /* ASKPASS_PROGRAM */

/* Path to rand helper */
extern char *SSH_RAND_HELPER;

/* Default user path that includes location of scp. */
extern char *_PATH_STDPATH_WITH_SCP;
