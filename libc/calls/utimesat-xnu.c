/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Jingyi Xie                                                    │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"

// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/getattrlist.2.html
typedef uint32_t attrgroup_t;
struct attrlist {
  uint16_t bitmapcount;   /* number of attr. bit sets in list */
  uint16_t reserved;      /* (to maintain 4-byte alignment) */
  attrgroup_t commonattr; /* common attribute group */
  attrgroup_t volattr;    /* volume attribute group */
  attrgroup_t dirattr;    /* directory attribute group */
  attrgroup_t fileattr;   /* file attribute group */
  attrgroup_t forkattr;   /* fork attribute group */
};
#define ATTR_BIT_MAP_COUNT 5

// https://github.com/golang/go/blob/d90b98e65320778f3b1f99a6951ab20f04d218b3/src/cmd/vendor/golang.org/x/sys/unix/zerrors_darwin_amd64.go#L74
#define ATTR_CMN_MODTIME 0x400
// https://github.com/golang/go/blob/d90b98e65320778f3b1f99a6951ab20f04d218b3/src/cmd/vendor/golang.org/x/sys/unix/zerrors_darwin_amd64.go#L56
#define ATTR_CMN_ACCTIME 0x1000

// https://github.com/golang/go/blob/d90b98e65320778f3b1f99a6951ab20f04d218b3/src/cmd/vendor/golang.org/x/sys/unix/zerrors_darwin_amd64.go#L486
#define FSOPT_NOFOLLOW 0x1

// https://keith.github.io/xcode-man-pages/setattrlist.2.html
int fsetattrlist(int fd, struct attrlist *attrList, void *attrBuf,
                 size_t attrBufSize, unsigned long options);
int setattrlistat(int dir_fd, const char *path, struct attrlist *attrList,
                  void *attrBuf, size_t attrBufSize, uint32_t options);

/* References:
 * https://github.com/bytecodealliance/rustix/issues/157
 * https://github.com/apple/darwin-xnu/blob/2ff845c2e033bd0ff64b5b6aa6063a1f8f65aa32/libsyscall/wrappers/utimensat.c
 */
int sys_utimensat_xnu(int dirfd, const char *path, const struct timespec ts[2],
                      int flags) {
  struct timespec times_in[2] = {0};
  if (ts) {
    times_in[0] = ts[0];
    times_in[1] = ts[1];
  } else {
    times_in[0].tv_nsec = UTIME_NOW;
    times_in[1].tv_nsec = UTIME_NOW;
  }
  struct timespec times_out[2] = {0};
  int attrs = 0;
  int attrbufsize = 0;

  struct timespec now;
  if (times_in[0].tv_nsec == UTIME_NOW || times_in[1].tv_nsec == UTIME_NOW) {
    struct timeval now_val;
    unassert(!gettimeofday(&now_val, 0));
    now = timeval_totimespec(now_val);
  }
  if (times_in[0].tv_nsec == UTIME_NOW) {
    times_in[0] = now;
  }
  if (times_in[1].tv_nsec == UTIME_NOW) {
    times_in[1] = now;
  }
  struct timespec *times_cursor = times_out;
  if (times_in[1].tv_nsec != UTIME_OMIT) {
    attrs |= ATTR_CMN_MODTIME;
    *times_cursor++ = times_in[1];
    attrbufsize += sizeof(struct timespec);
  }
  if (times_in[0].tv_nsec != UTIME_OMIT) {
    attrs |= ATTR_CMN_ACCTIME;
    *times_cursor = times_in[0];
    attrbufsize += sizeof(struct timespec);
  }
  struct attrlist attrlist = {
      .bitmapcount = ATTR_BIT_MAP_COUNT,
      .commonattr = attrs,
  };
  int flags_out = 0;
  if (flags & AT_SYMLINK_NOFOLLOW) {
    flags_out |= FSOPT_NOFOLLOW;
  }
  if (path) {
    return setattrlistat(dirfd, path, &attrlist, times_out, attrbufsize,
                         flags_out);
  } else {
    return fsetattrlist(dirfd, &attrlist, times_out, attrbufsize, flags_out);
  }
}