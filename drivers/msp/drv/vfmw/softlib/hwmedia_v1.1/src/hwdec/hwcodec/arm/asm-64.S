/*
 * Copyright (c) 2008 Mans Rullgard <mans@mansr.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifdef __ELF__
#   define ELF
#else
#   define ELF  #
#endif

#if 1
#   define FUNC
#else
#   define FUNC #
#endif

.macro  function name, export=0, align=2
    .macro endfunc
ELF     .size   \name, . - \name
FUNC    .endfunc
        .purgem endfunc
    .endm
        .text
        .align          \align
    .if \export
        .global \name
ELF     .type   \name, %function
FUNC    .func   \name
\name:
    .else
ELF     .type   \name, %function
FUNC    .func   \name
\name:
    .endif
.endm

.macro  const   name, align=2
    .macro endconst
ELF     .size   \name, . - \name
        .purgem endconst
    .endm
#ifdef __MACH__
        .section        .rodata
#else
        .const_data
#endif
        .align          \align
\name:
.endm

.macro  movrel rd, val
#if CONFIG_PIC && defined(__APPLE__)
        adrp            \rd, \val@PAGE
        add             \rd, \rd, \val@PAGEOFF
#elif CONFIG_PIC
        adrp            \rd, \val
        add             \rd, \rd, :lo12:\val
#else
        ldr             \rd, =\val
#endif
.endm

#define GLUE(a, b) a ## b
#define JOIN(a, b) GLUE(a, b)
#define X(s) JOIN(EXTERN_ASM, s)
