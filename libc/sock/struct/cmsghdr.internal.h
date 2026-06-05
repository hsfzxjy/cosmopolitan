#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_

#include "libc/sock/struct/msghdr.h"

COSMOPOLITAN_C_START_

struct cmsghdr_bsd {
  uint32_t cmsg_len;
  int32_t cmsg_level;
  int32_t cmsg_type;
};

int cmsghdr2bsd(const struct msghdr *msghdr, void **out, void *buf,
                size_t buflen, void *(*alloc_fn)(size_t));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_ */
