/* $OpenBSD: version.h,v 1.68 2013/11/08 01:38:11 djm Exp $ */

#ifdef GSI
#define GSI_VERSION	" GSI"
#else
#define GSI_VERSION	""
#endif

#ifdef KRB5
#define KRB5_VERSION	" KRB5"
#else
#define KRB5_VERSION	""
#endif

#ifdef MECHGLUE
#define MGLUE_VERSION	" MECHGLUE"
#else
#define MGLUE_VERSION	""
#endif

#define GPT_VERSION	" GSI_GSSAPI_GPT_5.7"

#define SSH_VERSION	"OpenSSH_6.4"

#ifdef NERSC_MOD
#define SSH_AUDITING	"NMOD_3.12"
#else
#define SSH_AUDITING	""
#endif /* NERSC_MOD */

#define SSH_PORTABLE	"p1"
#define SSH_HPN                "-hpn14v1"

#define SSH_RELEASE	SSH_VERSION SSH_PORTABLE SSH_HPN \
            GPT_VERSION GSI_VERSION KRB5_VERSION MGLUE_VERSION
