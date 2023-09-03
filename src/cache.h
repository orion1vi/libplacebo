/*
 * This file is part of libplacebo.
 *
 * libplacebo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libplacebo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libplacebo. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "common.h"

#include <libplacebo/cache.h>

// Convenience wrapper around pl_cache_set
static inline void pl_cache_str(pl_cache cache, uint64_t key, pl_str *str)
{
    pl_cache_set(cache, &(pl_cache_obj) {
        .key  = key,
        .data = pl_steal(NULL, str->buf),
        .size = str->len,
        .free = pl_free,
    });
    *str = (pl_str) {0};
}

// Steal and insert a cache object
static inline void pl_cache_steal(pl_cache cache, pl_cache_obj *obj)
{
    pl_assert(!obj->data || obj->free == pl_free);
    obj->data = pl_steal(NULL, obj->data);
    pl_cache_set(cache, obj);
}

// Resize `obj->data` to a given size, re-using allocated buffers where possible
static inline void pl_cache_obj_resize(void *alloc, pl_cache_obj *obj, size_t size)
{
    if (obj->free != pl_free) {
        if (obj->free)
            obj->free(obj->data);
        obj->data = pl_alloc(alloc, size);
        obj->free = pl_free;
    } else if (pl_get_size(obj->data) < size) {
        obj->data = pl_steal(alloc, obj->data);
        obj->data = pl_realloc(alloc, obj->data, size);
    }
    obj->size = size;
}