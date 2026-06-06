#ifndef COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
#define COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
COSMOPOLITAN_C_START_

#include "libc/intrin/atomic.h"
#undef __raise

/**
 * @fileoverview System DSO interfaces provided by APE loader.
 *
 * These functions are owned by the platform C library. Regardless of
 * platform, POSIX APIs returning `long` will follow the Linux Kernel
 * `-errno` convention, and hence should be wrapped with `_sysret()`.
 */

#define SYSLIB_MAGIC ('s' | 'l' << 8 | 'i' << 16 | 'b' << 24)

#define SYSLIB_VERSION 10 /* sync with ape/ape-m1.c */

/* if this number increases, then everyone on macos arm will need to
   reinstall ape loader in order to run newer ape binaries so please
   don't do this if it's sufficient to just check __syslib->version. */
#define SYSLIB_VERSION_MANDATORY 8

typedef uint64_t dispatch_time_t;
typedef uint64_t dispatch_semaphore_t;

struct Syslib {
  int __magic;
  int __version;
  long (*__fork)(void);
  long (*__pipe)(int[2]);
  long (*__clock_gettime)(int, void *);
  long (*__nanosleep)(const void *, void *);
  long (*__mmap)(void *, size_t, int, int, int, int64_t);
  int (*__pthread_jit_write_protect_supported_np)(void);
  void (*__pthread_jit_write_protect_np)(int);
  void (*__sys_icache_invalidate)(void *, size_t);
  int (*__pthread_create)(void *, const void *, void *(*)(void *), void *);
  void (*__pthread_exit)(void *);
  int (*__pthread_kill)(long, int);
  int (*__pthread_sigmask)(int, const void *restrict, void *restrict);
  int (*__pthread_setname_np)(const char *);
  dispatch_semaphore_t (*__dispatch_semaphore_create)(long);
  long (*__dispatch_semaphore_signal)(dispatch_semaphore_t);
  long (*__dispatch_semaphore_wait)(dispatch_semaphore_t, dispatch_time_t);
  dispatch_time_t (*__dispatch_walltime)(const void *, int64_t);
  /* v2 (2023-09-10) */
  long (*__pthread_self)(void);
  void (*__dispatch_release)(dispatch_semaphore_t);
  int (*__raise)(int);
  int (*__pthread_join)(long, void **);
  void (*__pthread_yield_np)(void);
  int __pthread_stack_min;
  int __sizeof_pthread_attr_t;
  int (*__pthread_attr_init)(void *);
  int (*__pthread_attr_destroy)(void *);
  int (*__pthread_attr_setstacksize)(void *, size_t);
  int (*__pthread_attr_setguardsize)(void *, size_t);
  /* v4 (2023-09-19) */
  void (*__exit)(int);
  long (*__close)(int);
  long (*__munmap)(void *, size_t);
  long (*__openat)(int, const void *, int, int);
  long (*__write)(int, const void *, size_t);
  long (*__read)(int, void *, size_t);
  long (*__sigaction)(int, const void *, void *);
  long (*__pselect)(int, void *, void *, void *, const void *, const void *);
  long (*__mprotect)(void *, size_t, int);
  /* v5 (2023-10-09) */
  long (*__sigaltstack)(const void *, void *);
  long (*__getentropy)(void *, size_t);
  long (*__sem_open)(const char *, int, uint16_t, unsigned);
  long (*__sem_unlink)(const char *);
  long (*__sem_close)(int *);
  long (*__sem_post)(int *);
  long (*__sem_wait)(int *);
  long (*__sem_trywait)(int *);
  long (*__getrlimit)(int, void *);
  long (*__setrlimit)(int, const void *);
  /* v6 (2023-11-03) */
  void *(*__dlopen)(const char *, int);
  void *(*__dlsym)(void *, const char *);
  int (*__dlclose)(void *);
  char *(*__dlerror)(void);
  /* v9 (2024-01-31) */
  int (*__pthread_cpu_number_np)(size_t *);
  /* v10 (2024-05-02) */
  long (*__sysctl)(int *, unsigned, void *, size_t *, void *, size_t);
  long (*__sysctlbyname)(const char *, void *, size_t *, void *, size_t);
  long (*__sysctlnametomib)(const char *, int *, size_t *);
};

#define UU(ret, name, args) typedef ret(*__syslib2_##name##_t) args;
#include "libc/runtime/syslib2.inc"
#undef UU

extern struct Syslib *__syslib;

struct TinySyslib {
  void *(*__dlopen)(const char *, int);
  void *(*__dlsym)(void *, const char *);
  int (*__dlclose)(void *);
  char *(*__dlerror)(void);
  int *(*__error)(void);
};

extern struct TinySyslib *__tinysyslib;

#define SLIB2(NAME)                                                      \
  ({                                                                     \
    extern void *__syslib2_func_ptr_##NAME;                              \
    extern void *__syslib2_load_ptr_##NAME(void);                        \
    void *__ptr = atomic_load_explicit(&__syslib2_func_ptr_##NAME,       \
                                       memory_order_acquire);            \
    (__syslib2_##NAME##_t)(__ptr ? __ptr : __syslib2_load_ptr_##NAME()); \
  })

#define SLIB2_ERRNO ((long)((*__tinysyslib->__error())))

#define SLIB2_CALL_V(F, ...)          \
  ({                                  \
    long __res = (F)(__VA_ARGS__);    \
    __res < 0 ? -SLIB2_ERRNO : __res; \
  })

#define SLIB2_CALL_N(N, ...)            \
  ({                                    \
    long __res = SLIB2(N)(__VA_ARGS__); \
    __res < 0 ? -SLIB2_ERRNO : __res;   \
  })

#define SLIB2_CACHE(VAR, NAME) __syslib2_##NAME##_t VAR = SLIB2(NAME)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_ */
