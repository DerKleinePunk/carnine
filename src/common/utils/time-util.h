/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

/***
  This file is part of systemd.
  Copyright 2010 Lennart Poettering
    //https://github.com/systemd/systemd/blob/4d09e1c8bab1d684172b1f277f3213825b30d2d9/src/basic/time-util.h
***/

#include <cstdlib>
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <unistd.h>     // UNIX standard function definitions
#include <signal.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h> 
#include <sys/timex.h>

typedef uint64_t usec_t;
typedef uint64_t nsec_t;

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

#define USEC_INFINITY ((usec_t) -1)
#define NSEC_INFINITY ((nsec_t) -1)

#define USEC_PER_MINUTE ((usec_t) (60ULL*USEC_PER_SEC))
#define USEC_PER_SEC ((usec_t) 1000000ULL)
#define NSEC_PER_USEC ((nsec_t) 1000ULL)

/* We assume a maximum timezone length of 6. TZNAME_MAX is not defined on Linux, but glibc internally initializes this
 * to 6. Let's rely on that. */
#define FORMAT_TIMESTAMP_MAX (3+1+10+1+8+1+6+1+6+1)

static inline bool isempty(const char *p) {
        return !p || !p[0];
}

usec_t timespec_load(const struct timespec *ts);
usec_t now(clockid_t clock_id);
struct tm *localtime_or_gmtime_r(const time_t *t, struct tm *tm, bool utc);
//static char *format_timestamp_internal( char *buf, size_t l, usec_t t, bool utc,bool us); 
char *format_timestamp(char *buf, size_t l, usec_t t);

#if SIZEOF_TIME_T == 8
/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit year
 * territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX ((usec_t) 253402214399000000)
#elif SIZEOF_TIME_T == 4
/* With a 32bit time_t we can't go beyond 2038... */
#define USEC_TIMESTAMP_FORMATTABLE_MAX ((usec_t) 2147483647000000)
#else
#error "Yuck, time_t is neither 4 not 8 bytes wide?"
#endif