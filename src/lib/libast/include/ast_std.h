/***********************************************************************
 *                                                                      *
 *               This software is part of the ast package               *
 *          Copyright (c) 1985-2013 AT&T Intellectual Property          *
 *                      and is licensed under the                       *
 *                 Eclipse Public License, Version 1.0                  *
 *                    by AT&T Intellectual Property                     *
 *                                                                      *
 *                A copy of the License is available at                 *
 *          http://www.eclipse.org/org/documents/epl-v10.html           *
 *         (with md5 checksum b35adb5213ca9657e911e9befb180842)         *
 *                                                                      *
 *              Information and Software Systems Research               *
 *                            AT&T Research                             *
 *                           Florham Park NJ                            *
 *                                                                      *
 *               Glenn Fowler <glenn.s.fowler@gmail.com>                *
 *                    David Korn <dgkorn@gmail.com>                     *
 *                     Phong Vo <phongvo@gmail.com>                     *
 *                                                                      *
 ***********************************************************************/
/*
 * Advanced Software Technology Library
 * AT&T Research
 *
 * a union of standard headers that works
 * with local extensions enabled
 * and local omission compensation
 */
#ifndef _AST_STD_H
#define _AST_STD_H 1

#include <iconv.h>
#include <stdbool.h>

#include "ast_fcntl.h"
#include "ast_sys.h"

/*
 * <stdio.h> and <wchar.h> are entangled on some systems
 * and this make <stdio.h> pop up in weird places
 * this dance works around those places
 */

#if _AST_H
#include "sfio.h" /* moved from <ast.h> because mbstate_t entangled with <stdio.h> on some systems */
#endif
#include <wchar.h>

#undef setlocale
#define setlocale _ast_setlocale

extern char *setlocale(int, const char *);
extern char *strerror(int);

#define AST_MESSAGE_SET 3 /* see <mc.h> mcindex()		*/

typedef struct Mbstate_s {
    mbstate_t mb_state;
    int mb_errno;
} Mbstate_t;

typedef struct {
    char *id;

    struct {
        uint32_t serial;
        uint32_t set;
        bool is_utf8;  // true if current locale uses UTF-8 for its encoding
    } locale;

    int tmp_int;
    wchar_t tmp_wchar;

    int (*mb_width)(wchar_t);

    uint32_t env_serial;
    uint32_t mb_sync;
    uint32_t version;

    int (*mb_alpha)(wchar_t);

    int byte_max;

    iconv_t mb_uc2wc;
    iconv_t mb_wc2uc;
} _Ast_info_t;

extern _Ast_info_t _ast_info;

/* and now introducing prototypes botched by the standard(s) */

extern char *translate(const char *, const char *, const char *, const char *);

#endif  // _AST_STD_H
