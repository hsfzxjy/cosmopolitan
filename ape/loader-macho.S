/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macho.h"
#include "libc/sysv/consts/prot.h"
#include "libc/dce.h"
#include "libc/macros.h"

.equ NIMPORTS, 5
.equ CHAINED_FIXUPS_SIZE, chained_fixups_end - chained_fixups_start
#if defined(__aarch64__)
.equ PAGE_SIZE, 0x4000
.equ PAGE_ZERO_SIZE, 0x100000000
.equ MACHO_FLAG, MAC_DYLDLINK|MAC_BINDATLOAD|MAC_TWOLEVEL|MAC_PIE
#elif defined(__x86_64__)
.equ PAGE_SIZE, 0x1000
.equ PAGE_ZERO_SIZE, 0x200000
.equ MACHO_FLAG, MAC_SPLIT_SEGS|MAC_DYLDLINK
#endif

//	Apple Mach-O Executable Headers
//	Fixups are applied by objbincopy (no need now)
//	There must exist a MAC_LC_SEGMENT_64 for every PT_LOAD

	.section .macho,"a",@progbits
	.balign	64
	.long	0xFEEDFACE+1
#if defined(__x86_64__)
	.long	MAC_CPU_NEXGEN32E
	.long	MAC_CPU_NEXGEN32E_ALL
#elif defined(__aarch64__)
	.long	MAC_CPU_ARM64
	.long	MAC_CPU_ARM64_ALL
#endif
	.long	MAC_EXECUTE
	.long 10                  // number of load commands
	.long __end_of_macho-10f  // size of all load commands
	.long MACHO_FLAG          // flags
	.long 0                   // reserved
10:
	.long	MAC_LC_SEGMENT_64
	.long  20f-10b                             // unmaps first page dir
	.ascin "__PAGEZERO", 16                    // consistent with linux
	.quad  0,            PAGE_ZERO_SIZE, 0, 0  // vaddr/memsz/file offset/file size
	.long  0,            0,              0, 0  // maxprot/initprot/section count/flags
20:
	.long	MAC_LC_SEGMENT_64
	.long	30f-20b
	.ascin	"__TEXT",16
	.quad SEG_TEXT_VADDR                // vaddr
	.quad SEG_TEXT_MEMSZ                // memsz
	.quad SEG_TEXT_OFFST                // file offset
	.quad SEG_TEXT_FILSZ                // file size
	.long VM_PROT_READ|VM_PROT_EXECUTE  // maxprot
	.long VM_PROT_READ|VM_PROT_EXECUTE  // initprot
	.long 0                             // segment section count
	.long 0                             // flags
30:
	.long	MAC_LC_SEGMENT_64
	.long	40f-30b
	.ascin	"__DATA_CONST",16
	.quad SEG_DATA_CONST_VADDR        // vaddr
	.quad SEG_DATA_CONST_MEMSZ        // memsz
	.quad SEG_DATA_CONST_OFFST        // file offset
	.quad SEG_DATA_CONST_FILSZ        // file size
	.long VM_PROT_READ|VM_PROT_WRITE  // maxprot
	.long VM_PROT_READ|VM_PROT_WRITE  // initprot
	.long 1                           // segment section count
	.long 0                           // flags

	/// section
	.ascin  "__got",16
	.ascin  "__DATA_CONST",16
	.quad SEG_DATA_CONST_VADDR  // vaddr
	.quad 8*NIMPORTS            // memsz
	.long SEG_DATA_CONST_OFFST  // file offset
	.long 3                     // alignlog2, 3 means 8-byte alignment
	.long 0                     // relotaboff
	.long 0                     // nreloc
	.long 6                     // flags, 6 = S_NON_LAZY_SYMBOL_POINTERS
	.long 0                     // reserved1
	.long 0                     // reserved2
	.long 0                     // reserved3
40:
	.long	MAC_LC_SEGMENT_64
	.long	50f-40b
	.ascin	"__LINKEDIT",16
	.quad SEG_LINKEDIT_VADDR  // vaddr
	.quad SEG_LINKEDIT_MEMSZ  // memsz
	.quad SEG_LINKEDIT_OFFST  // file offset
	.quad SEG_LINKEDIT_FILSZ  // file size
	.long VM_PROT_READ        // maxprot
	.long VM_PROT_READ        // initprot
	.long 0                   // segment section count
	.long 0                   // flags
50:
	.long	MAC_LC_UUID
	.long	60f-50b
	.quad 0x4527148ba7a513ef  // uuid1
	.quad 0x56fa865940665e8f  // uuid2
#if defined(__x86_64__)
60:
	.long	MAC_LC_UNIXTHREAD
	.long 70f-60b               // cmdsize
	.long MAC_THREAD_NEXGEN32E  // flavaflav
	.long (620f-610f)/4         // count
610:
	.quad 0              // rax
	.quad 0              // rbx
	.quad 0              // rcx
	.quad 0              // rdx
	.quad 0              // rdi
	.quad 0              // rsi
	.quad 0              // rbp
	.quad 0              // rsp
	.quad 0              // r8
	.quad 0              // r9
	.quad 0              // r10
	.quad 0              // r11
	.quad 0              // r12
	.quad 0              // r13
	.quad 0              // r14
	.quad 0              // r15
	.quad XnuEntrypoint  // rip
	.quad 0              // rflags
	.quad 0              // cs
	.quad 0              // fs
	.quad 0              // gs
620:
#elif defined(__aarch64__)
60:
	.long	MAC_LC_MAIN
	.long	70f-60b
	.quad XNUENTRY_OFFSET  // entry offset to start of __TEXT
	.quad 0                // stack_size, 0=default
#endif
70:
	.long	MAC_LC_LOAD_DYLINKER
	.long	80f-70b
	.long	12
	.ascin	"/usr/lib/dyld",20
80:
	.long MAC_LC_LOAD_DYLIB
	.long 90f-80b
	.long 24       // name offset
	.long 2        // timestamp, not important
	.long 0x10000  // current version 1.0.0
	.long 0x10000  // compatibility version 1.0.0
	.ascin "/usr/lib/libSystem.B.dylib",32
90:
	.long MAC_LC_BUILD_VERSION
	.long 100f-90b
	.long 1           // platform, PLATFORM_MACOS
	.long 0x000b0000  // minos = 11.0.0
	.long 0x000b0000  // sdk   = 11.0.0
	.long 0           // ntools
100:
	.long MAC_LC_DYLD_CHAINED_FIXUPS
	.long 110f-100b
	.long CHAINED_FIXUPS_OFFSET  // dataoff
	.long CHAINED_FIXUPS_SIZE    // datasize
110:
__end_of_macho:

.section .sort.rodata.mac_chained_fixups,"a",@progbits
	.balign 8
chained_fixups_start:
0:
	.long 0                   // fixups_version
	.long 1f-0b               // starts_offset
	.long chained_imports-0b  // imports_offset
	.long chained_symbols-0b  // symbols_offset
	.long 5                   // imports_count
	.long 1                   // imports_format DYLD_CHAINED_IMPORT
	.long 0                   // symbols_format
	.balign 8
1:
	.long 4        // seg_count
	.long 0        // seg_info_offset for __PAGE_ZERO
	.long 0        // seg_info_offset for __TEXT
	.long 1f - 1b  // seg_info_offset for __DATA_CONST
	.long 0        // seg_info_offset for .data_const
	.balign 8
1:
	.long  1f - 1b               // size
	.short PAGE_SIZE             // page_size
	.short 6                     // pointer_format DYLD_CHAINED_PTR_64
	.quad  SEG_DATA_CONST_OFFST  // segment_offset
	.long  0                     // max_valid_pointer
	.short 1                     // page_count
	.short 0                     // page_start[0]
1:
	.balign 8
chained_imports:
	.long 1 | ((chained_sym_dlopen-chained_symbols) << 9)
	.long 1 | ((chained_sym_dlsym-chained_symbols) << 9)
	.long 1 | ((chained_sym_dlclose-chained_symbols) << 9)
	.long 1 | ((chained_sym_dlerror-chained_symbols) << 9)
	.long 1 | ((chained_sym__error-chained_symbols) << 9)

chained_symbols:
	.balign 8,0
	.short 0x2
chained_sym_dlopen:
	.asciz "_dlopen"
chained_sym_dlsym:	
	.asciz "_dlsym"
chained_sym_dlclose:
	.asciz "_dlclose"
chained_sym_dlerror:
	.asciz "_dlerror"
chained_sym__error:
	.asciz "___error"
	.balign 8,0
chained_fixups_end:
.previous