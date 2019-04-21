/***********************************************************************
 *                                                                      *
 *               This software is part of the ast package               *
 *          Copyright (c) 1985-2011 AT&T Intellectual Property          *
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
#include "config_ast.h"  // IWYU pragma: keep

#include <stdlib.h>
#include <string.h>

#include "sfhdr.h"  // IWYU pragma: keep
#include "sfio.h"

/*      Swap two streams. If the second argument is NULL,
**      a new stream will be created. Always return the second argument
**      or the new stream. Note that this function will always work
**      unless streams are locked by SF_PUSH.
**
**      Written by Kiem-Phong Vo.
*/

Sfio_t *sfswap(Sfio_t *f1, Sfio_t *f2) {
    Sfio_t tmp;
    int f1pool, f2pool, f1mode, f2mode, f1flags, f2flags;

    if (!f1 || (f1->mode & SF_AVAIL) || (SFFROZEN(f1) && (f1->mode & SF_PUSH))) return NULL;
    if (f2 && SFFROZEN(f2) && (f2->mode & SF_PUSH)) return NULL;
    if (f1 == f2) return f2;

    f1mode = f1->mode;
    SFLOCK(f1, 0)
    f1->mode |= SF_PUSH; /* make sure there is no recursion on f1 */

    if (f2) {
        f2mode = f2->mode;
        SFLOCK(f2, 0)
        f2->mode |= SF_PUSH; /* make sure there is no recursion on f2 */
    } else {
        f2 = f1->file == 0 ? sfstdin : f1->file == 1 ? sfstdout : f1->file == 2 ? sfstderr : NULL;
        if ((!f2 || !(f2->mode & SF_AVAIL))) {
            f2 = malloc(sizeof(Sfio_t));
            if (!f2) {
                f1->mode = f1mode;
                SFOPEN(f1)
                return NULL;
            }

            SFCLEAR(f2, NULL);
        }
        f2->mode = SF_AVAIL | SF_LOCK;
        f2mode = SF_AVAIL;
    }

    if (!f1->pool) {
        f1pool = -1;
    } else {
        for (f1pool = f1->pool->n_sf - 1; f1pool >= 0; --f1pool) {
            if (f1->pool->sf[f1pool] == f1) break;
        }
    }
    if (!f2->pool) {
        f2pool = -1;
    } else {
        for (f2pool = f2->pool->n_sf - 1; f2pool >= 0; --f2pool) {
            if (f2->pool->sf[f2pool] == f2) break;
        }
    }

    f1flags = f1->flags;
    f2flags = f2->flags;

    /* swap image and pool entries */
    memcpy((void *)(&tmp), (void *)f1, sizeof(Sfio_t));
    memcpy((void *)f1, (void *)f2, sizeof(Sfio_t));
    memcpy((void *)f2, (void *)(&tmp), sizeof(Sfio_t));
    if (f2pool >= 0) f1->pool->sf[f2pool] = f1;
    if (f1pool >= 0) f2->pool->sf[f1pool] = f2;

    if (f2flags & SF_STATIC) {
        f2->flags |= SF_STATIC;
    } else {
        f2->flags &= ~SF_STATIC;
    }

    if (f1flags & SF_STATIC) {
        f1->flags |= SF_STATIC;
    } else {
        f1->flags &= ~SF_STATIC;
    }

    if (f2mode & SF_AVAIL) /* swapping to a closed stream */
    {
        if (!(f1->flags & SF_STATIC)) free(f1);
    } else {
        f1->mode = f2mode;
        SFOPEN(f1)
    }

    f2->mode = f1mode;
    SFOPEN(f2)
    return f2;
}
