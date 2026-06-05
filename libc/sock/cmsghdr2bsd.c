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
#include "libc/dce.h"
#include "libc/sock/struct/cmsghdr.h"
#include "libc/sock/struct/cmsghdr.internal.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define _ALIGN(len, a) (((len) + (a) - 1) & (size_t)~((a) - 1))
#define _SPACE(len, a) (_ALIGN(sizeof(struct cmsghdr_bsd), a) + _ALIGN(len, a))
#define _LEN(len, a)   (_ALIGN(sizeof(struct cmsghdr_bsd), a) + (len))
#define _DATA(cmsg, a) ((void *)(cmsg) + _ALIGN(sizeof(struct cmsghdr_bsd), a))

static inline int alignment() {
  if (IsXnu()) {
    return 4;
  }
#ifdef __aarch64__
  if (IsNetbsd()) {
    return 16;
  }
#endif
  return 8;
}

static inline size_t space_needed(const struct msghdr *msghdr, size_t a) {
  size_t space = 0;
  for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(msghdr); cmsg;
       cmsg = CMSG_NXTHDR(msghdr, cmsg)) {
    space += _SPACE(cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr)), a);
  }
  return space;
}

int cmsghdr2bsd(const struct msghdr *msghdr, void **out, void *buf,
                size_t buflen, void *(*alloc_fn)(size_t)) {
  size_t a = alignment();
  size_t space = space_needed(msghdr, a);
  void *base = buf;
  if (space > buflen) {
    base = alloc_fn(space);
    if (!base) {
      return enomem();
    }
  }
  void *ptr = base;
  for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(msghdr); cmsg;
       cmsg = CMSG_NXTHDR(msghdr, cmsg)) {
    struct cmsghdr_bsd *cmsg2 = (struct cmsghdr_bsd *)ptr;
    size_t data_len = cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr));
    cmsg2->cmsg_len = _LEN(data_len, a);
    cmsg2->cmsg_level = cmsg->cmsg_level;
    cmsg2->cmsg_type = cmsg->cmsg_type;
    memcpy(_DATA(cmsg2, a), CMSG_DATA(cmsg), data_len);
    ptr += _SPACE(data_len, a);
  }
  *out = (struct cmsghdr_bsd *)base;
  return space;
}
