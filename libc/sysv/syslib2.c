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
#include "libc/macros.h"
#include "libc/runtime/syslib.internal.h"

#define RTLD_XNU_DEFAULT ((void *)-2)
#define INVALID_SYMBOL   ((void *)1)

__privileged static void *__syslib2_lazy_load(const char *name, void **slot) {
  void *fn = NULL;
  fn = __tinysyslib->__dlsym(RTLD_XNU_DEFAULT, name);
  if (!fn) {
    fn = INVALID_SYMBOL;  // let it crash later when we try to call it
  }
  atomic_store_explicit(slot, fn, memory_order_release);
  return fn;
}

#define UU(ret, name, args)                                        \
  void *__syslib2_func_ptr_##name                                  \
      __attribute__((section(".bss.syslib2." #name))) = 0;         \
                                                                   \
  __attribute__((section(".privileged.syslib2." #name))) void *    \
  __syslib2_load_ptr_##name(void) {                                \
    return __syslib2_lazy_load(#name, &__syslib2_func_ptr_##name); \
  }
#include "libc/runtime/syslib2.inc"
#undef UU
