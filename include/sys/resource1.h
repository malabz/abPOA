// this code from https://github.com/JellyBrick/sys-resource-win and https://github.com/openvswitch/ovs
// using Apache-2.0 LICENSE

#ifndef  __RESOURCE_H__
#define  __RESOURCE_H__

#include <Winsock2.h>
#include <Psapi.h>
#include <windows.h>
#include <stdio.h>

#define RLIMIT_CPU      0   /* limit on CPU time per process */
#define RLIMIT_FSIZE    1   /* limit on file size */
#define RLIMIT_DATA     2   /* limit on data segment size */
#define RLIMIT_STACK    3   /* limit on process stack size */
#define RLIMIT_CORE     4   /* limit on size of core dump file */
#define RLIMIT_NOFILE   5   /* limit on number of open files */
#define RLIMIT_AS       6   /* limit on process total address space 
size */
#define RLIMIT_VMEM     RLIMIT_AS

#define RLIM_NLIMITS    7


/*
 * process resource limits definitions
 */

struct rlimit {
//        LARGE_INTEGER  rlim_cur;
//        LARGE_INTEGER  rlim_max;
          __int64    rlim_cur;
          __int64    rlim_max;
};

typedef struct rlimit  rlimit_t;

/*
 * Prototypes
 */
int getrlimit(int resource, struct rlimit *);
int setrlimit(int resource, const struct rlimit *);

size_t rfwrite( const void *buffer, size_t size, size_t count, FILE
*stream );
int _rwrite( int handle, const void *buffer, unsigned int count );

//
// Following are the prototypes for the real functions...
//
// size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream );
// int _write( int handle, const void *buffer, unsigned int count );


struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
    long   ru_maxrss;        /* maximum resident set size */
    long   ru_ixrss;         /* integral shared memory size */
    long   ru_idrss;         /* integral unshared data size */
    long   ru_isrss;         /* integral unshared stack size */
    long   ru_minflt;        /* page reclaims (soft page faults) */
    long   ru_majflt;        /* page faults (hard page faults) */
    long   ru_nswap;         /* swaps */
    long   ru_inblock;       /* block input operations */
    long   ru_oublock;       /* block output operations */
    long   ru_msgsnd;        /* IPC messages sent */
    long   ru_msgrcv;        /* IPC messages received */
    long   ru_nsignals;      /* signals received */
    long   ru_nvcsw;         /* voluntary context switches */
    long   ru_nivcsw;        /* involuntary context switches */
};

#ifndef RUSAGE_SELF
#define RUSAGE_SELF 1
#endif

#ifndef RUSAGE_CHILDREN
#define RUSAGE_CHILDREN 2
#endif

#ifndef RUSAGE_THREAD
#define RUSAGE_THREAD 3
#endif



int getrusage(int who, struct rusage *rusage);


#endif