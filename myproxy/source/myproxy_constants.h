/*
 * myproxy_constants.h
 *
 * constant declarations
 */
#ifndef __MYPROXY_CONSTANTS_H
#define __MYPROXY_CONSTANTS_H

/* Maximum pass phrase length */
#define MAX_PASS_LEN  1024 /* Arbitrary */

/* Define default myproxy-server -- should probably be put in config file */
#define MYPROXY_SERVER_PORT            7512

/* specify maximum delegation lifetime allowed on myproxy-server */
#define MYPROXY_DEFAULT_HOURS          168     /* 1 week */
#define MYPROXY_DEFAULT_DELEG_HOURS    12

#define MYPROXY_DEFAULT_KEYBITS        2048    /* NIST SP 800-57 */

#define MYPROXY_DEFAULT_TIMEOUT        120

#define MYPROXY_DEFAULT_CLOCK_SKEW     300     /* 5 minutes */

#define MYPROXY_CREDS_MAX_NAMELEN      80      /* longer names are
                                                  hashed when used in
                                                  filenames */

/* myproxy client protocol information */
/* beware no string below may be a suffix of another */
#define MYPROXY_VERSION_STRING      "VERSION="
#define MYPROXY_COMMAND_STRING      "COMMAND="
#define MYPROXY_USERNAME_STRING     "USERNAME="
#define MYPROXY_PASSPHRASE_STRING   "PASSPHRASE="
#define MYPROXY_NEW_PASSPHRASE_STRING "NEW_PHRASE="
#define MYPROXY_LIFETIME_STRING     "LIFETIME="
#define MYPROXY_RETRIEVER_STRING     "RETRIEVER="
#define MYPROXY_TRUSTED_RETRIEVER_STRING     "RETRIEVER_TRUSTED="
#define MYPROXY_KEY_RETRIEVER_STRING     "KEYRETRIEVERS="
#define MYPROXY_RENEWER_STRING     "RENEWER="
#define MYPROXY_CRED_NAME_STRING   "NAME="
#define MYPROXY_CRED_DESC_STRING   "DESC="
#define MYPROXY_AUTHORIZATION_STRING "AUTHORIZATION_DATA="
#define MYPROXY_ADDITIONAL_CREDS_STRING "ADDL_CREDS="
#define MYPROXY_LOCKMSG_STRING     "LOCKMSG="
#define MYPROXY_CRED_PREFIX	    "CRED"
#define MYPROXY_START_TIME_STRING   "START_TIME="
#define MYPROXY_END_TIME_STRING     "END_TIME="
#define MYPROXY_CRED_OWNER_STRING   "OWNER="
#define MYPROXY_TRUSTED_CERTS_STRING "TRUSTED_CERTS="
#define MYPROXY_FILEDATA_PREFIX     "FILEDATA"
#define MYPROXY_VONAME_STRING      "VONAME="
#define MYPROXY_VOMSES_STRING      "VOMSES="

/* myproxy server protocol information */
#define MYPROXY_RESPONSE_TYPE_STRING     "RESPONSE="
#define MYPROXY_RESPONSE_SIZE_STRING     "RESPONSE_SIZE="
#define MYPROXY_RESPONSE_STRING   "RESPONSE_STR="
#define MYPROXY_ERROR_STRING        "ERROR="

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN	46
#endif

#endif /* __MYPROXY_CONSTANTS_H */
