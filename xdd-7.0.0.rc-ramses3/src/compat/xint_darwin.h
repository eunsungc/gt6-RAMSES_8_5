/*
 * XDD - a data movement and benchmarking toolkit
 *
 * Copyright (C) 1992-2013 I/O Performance, Inc.
 * Copyright (C) 2009-2013 UT-Battelle, LLC
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */
#ifndef XDD_DARWIN_H
#define XDD_DARWIN_H

#include "config.h"

/* Standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>

/* POSIX headers */
#include <sys/types.h>
#include <unistd.h> /* UNIX Only */
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/resource.h> /* needed for multiple processes */
#include <pthread.h>
#include <sched.h>
#include <sys/stat.h>
#include <string.h>
/* for the global clock stuff */
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

/* Platform headers */
#include <libgen.h>
#if (SNFS)
#include <client/cvdrfile.h>
#endif

/* XDD internal compatibility headers for this platform */
#include "xint_barrier.h"
#include "xint_nclk.h" /* nclk_t, prototype compatibility */
#include "xint_misc.h"

#define MP_MUSTRUN 1 /* ASsign this thread to a specific processor */
#define MP_NPROCS 2 /* return the number of processors on the system */
typedef int  sd_t;  /* A socket descriptor */
#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif

/* ------------------------------------------------------------------
 * Constants
 * ------------------------------------------------------------------ */
#define DFL_FL_ADDR INADDR_ANY /* Any address */  /* server only */
#define closesocket(sd) close(sd)
//static bool  sockets_init(void);
#include "xint_plan.h"

#endif
