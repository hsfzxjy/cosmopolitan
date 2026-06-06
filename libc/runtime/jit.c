/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"

__privileged static void pthread_jit_write_protect_np_workaround(int enabled) {
#ifdef __aarch64__
/* from the xnu codebase */
#define _COMM_PAGE_START_ADDRESS      0x0000000FFFFFC000ul
#define _COMM_PAGE_APRR_SUPPORT       (_COMM_PAGE_START_ADDRESS + 0x10C)
#define _COMM_PAGE_APRR_WRITE_ENABLE  (_COMM_PAGE_START_ADDRESS + 0x110)
#define _COMM_PAGE_APRR_WRITE_DISABLE (_COMM_PAGE_START_ADDRESS + 0x118)

  struct timespec ts = {
      .tv_sec = 0,
      .tv_nsec = 10 * 1000,  // 10 microseconds = 10,000 nanoseconds
  };

  int count_start = 8192;
  volatile int count = count_start;
  unsigned long *addr, val, val2, reread = -1;
  addr = (unsigned long *)(!enabled ? _COMM_PAGE_APRR_WRITE_ENABLE
                                    : _COMM_PAGE_APRR_WRITE_DISABLE);
  switch (*(volatile unsigned char *)_COMM_PAGE_APRR_SUPPORT) {
    case 1:
      do {
        val = *addr;
        reread = -1;
        __asm__ volatile("msr\tS3_4_c15_c2_7,%0\n"
                         "isb\tsy\n"
                         : /* no outputs */
                         : "r"(val)
                         : "memory");
        val2 = *addr;
        __asm__ volatile("mrs\t%0,S3_4_c15_c2_7\n"
                         : "=r"(reread)
                         : /* no inputs */
                         : "memory");
        if (val2 == reread) {
          return;
        }
        SLIB2(nanosleep)(&ts, NULL);
      } while (count-- > 0);
      break;
    case 3:
      do {
        val = *addr;
        reread = -1;
        __asm__ volatile("msr\tS3_6_c15_c1_5,%0\n"
                         "isb\tsy\n"
                         : /* no outputs */
                         : "r"(val)
                         : "memory");
        val2 = *addr;
        __asm__ volatile("mrs\t%0,S3_6_c15_c1_5\n"
                         : "=r"(reread)
                         : /* no inputs */
                         : "memory");
        if (val2 == reread) {
          return;
        }
        SLIB2(nanosleep)(&ts, NULL);
      } while (count-- > 0);
      break;
    default:
      SLIB2(pthread_jit_write_protect_np)(enabled);
      return;
  }
  SLIB2(exit)(12);
#endif
}

__privileged void __jit_begin(void) {
  if (IsXnu()) {
    if (SLIB2(pthread_jit_write_protect_supported_np)()) {
      pthread_jit_write_protect_np_workaround(false);
    }
  }
}

__privileged void __jit_end(void) {
  if (IsXnu()) {
    if (SLIB2(pthread_jit_write_protect_supported_np)()) {
      pthread_jit_write_protect_np_workaround(true);
    }
  }
}
