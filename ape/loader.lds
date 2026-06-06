/*-*- mode: ld-script; indent-tabs-mode: nil; tab-width: 2; coding: utf-8   -*-│
│ vi: set et sts=2 sw=2 fenc=utf-8                                         :vi │
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

ENTRY(ElfEntrypoint)

PHDRS {
  text PT_LOAD FLAGS(1) FILEHDR PHDRS; /* PF_X */
  data PT_LOAD FLAGS(6); /* PF_W|PF_R */
  rodata PT_LOAD FLAGS(4); /* PF_R */
  stack PT_GNU_STACK FLAGS(6); /* PF_W|PF_R */
  note PT_NOTE FLAGS(4); /* PF_R */
}

SECTIONS {

  . = SEGMENT_START("text-segment", 0);
  __executable_start = .;
  __text_start = .;
  . += SIZEOF_HEADERS;

  .macho : {
    KEEP(*(.macho))
  } :text

  .note : {
    KEEP(*(.note))
  } :text :note

  .text : {
    *(.text .text.* .gnu.linkonce.t.*)
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    . = ALIGN(CONSTANT(COMMONPAGESIZE));
    __text_end = .;
  } :text

  .data ALIGN(CONSTANT(COMMONPAGESIZE)) : {
    __data_const_start = .;
    *(.data .data.*)
    *(.got .got.*)
    . = ALIGN(CONSTANT(COMMONPAGESIZE));
    __data_const_end = .;
  } :data

  .mac_chained_fixups ALIGN(CONSTANT(COMMONPAGESIZE)) : {
    __linkedit_start = .;
    __chained_fixups_start = .;
    KEEP(*(.sort.rodata.mac_chained_fixups))
    __linkedit_end = .;
  } :rodata

  .stack : {
    *(.stack)
  } :stack

  _end = .;

  .stab               0 : { *(.stab) }
  .stabstr            0 : { *(.stabstr) }
  .stab.excl          0 : { *(.stab.excl) }
  .stab.exclstr       0 : { *(.stab.exclstr) }
  .stab.index         0 : { *(.stab.index) }
  .stab.indexstr      0 : { *(.stab.indexstr) }
  .gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
  .debug              0 : { *(.debug) }
  .line               0 : { *(.line) }
  .debug_srcinfo      0 : { *(.debug_srcinfo) }
  .debug_sfnames      0 : { *(.debug_sfnames) }
  .debug_aranges      0 : { *(.debug_aranges) }
  .debug_pubnames     0 : { *(.debug_pubnames) }
  .debug_info         0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev       0 : { *(.debug_abbrev) }
  .debug_line         0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame        0 : { *(.debug_frame) }
  .debug_str          0 : { *(.debug_str) }
  .debug_loc          0 : { *(.debug_loc) }
  .debug_macinfo      0 : { *(.debug_macinfo) }
  .debug_weaknames    0 : { *(.debug_weaknames) }
  .debug_funcnames    0 : { *(.debug_funcnames) }
  .debug_typenames    0 : { *(.debug_typenames) }
  .debug_varnames     0 : { *(.debug_varnames) }
  .debug_pubtypes     0 : { *(.debug_pubtypes) }
  .debug_ranges       0 : { *(.debug_ranges) }
  .debug_macro        0 : { *(.debug_macro) }
  .debug_addr         0 : { *(.debug_addr) }
  .debug_loclists     0 : { *(.debug_loclists) }
  .debug_rnglists     0 : { *(.debug_rnglists) }
  .debug_line_str     0 : { *(.debug_line_str) }
  .debug_extra        0 : { *(.debug_line_str) }
  .debug_pubtypes     0 : { *(.debug_pubtypes) }
  .gnu.attributes     0 : { KEEP(*(.gnu.attributes)) }

  /DISCARD/ : {
    *(.*)
  }

  SEG_TEXT_VADDR = __text_start;
  SEG_TEXT_MEMSZ = ALIGN(__text_end - __text_start, CONSTANT(COMMONPAGESIZE));
  SEG_TEXT_OFFST = __text_start - __executable_start;
  SEG_TEXT_FILSZ = __text_end - __text_start;

  SEG_DATA_CONST_VADDR = __data_const_start;
  SEG_DATA_CONST_MEMSZ = ALIGN(__data_const_end - __data_const_start, CONSTANT(COMMONPAGESIZE));
  SEG_DATA_CONST_OFFST = __data_const_start - __executable_start;
  SEG_DATA_CONST_FILSZ = __data_const_end - __data_const_start;

  SEG_LINKEDIT_VADDR = __linkedit_start;
  SEG_LINKEDIT_MEMSZ = ALIGN(__linkedit_end - __linkedit_start, CONSTANT(COMMONPAGESIZE));
  SEG_LINKEDIT_OFFST = __linkedit_start - __executable_start;
  SEG_LINKEDIT_FILSZ = __linkedit_end - __linkedit_start;

  XNUENTRY_OFFSET = XnuEntrypoint - __text_start;

  CHAINED_FIXUPS_OFFSET = __chained_fixups_start - __executable_start;
}
