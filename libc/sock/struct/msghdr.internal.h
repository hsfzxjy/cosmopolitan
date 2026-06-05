#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_
#include "libc/sock/struct/msghdr.h"
COSMOPOLITAN_C_START_

struct msghdr_bsd {
  void *msg_name;
  uint32_t msg_namelen;
  struct iovec *msg_iov;
  uint32_t msg_iovlen; /* « different type */
  void *msg_control;
  uint32_t msg_controllen;
  uint32_t msg_flags; /* « different type */
};

static inline void msghdr2bsd(const struct msghdr *msg,
                              struct msghdr_bsd *out) {
  out->msg_name = msg->msg_name;
  out->msg_namelen = msg->msg_namelen;
  out->msg_iov = msg->msg_iov;
  out->msg_iovlen = msg->msg_iovlen;
  out->msg_control = msg->msg_control;
  out->msg_controllen = msg->msg_controllen;
  out->msg_flags = msg->msg_flags;
}

ssize_t sys_sendmsg(int, const struct msghdr *, int);
ssize_t sys_recvmsg(int, struct msghdr *, int);
bool __asan_is_valid_msghdr(const struct msghdr *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_ */
