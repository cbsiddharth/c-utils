/******************************************************************************

                  Copyright (c) 2018 EmbedJournal

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

    Author : Siddharth Chandrasekaran
    Email  : siddharth@embedjournal.com
    Date   : Fri Aug 10 11:22:11 IST 2018

******************************************************************************/
#ifndef __CIRCULAR_GENERIC_BUFFER_H_
#define __CIRCULAR_GENERIC_BUFFER_H_

#include <stdint.h>

/** --- Internal method and structures. DON'T USE --------------------------- */
typedef struct {
    void * const buffer;
    uint16_t push_count;
    uint16_t pop_count;
    uint16_t const size;
    uint16_t const element_size;
} circ_gbuf_t;

#define __CIRC_GBUF_V_DEF(type, buf, sz)          \
    type buf ## _circ_gbuf_data[sz];              \
    circ_gbuf_t buf = {                           \
        .buffer = buf ## _circ_gbuf_data,         \
        .push_count = 0,                          \
        .pop_count = 0,                           \
        .size = sz,                               \
        .element_size = sizeof(type)              \
    };

int __circ_gbuf_push(circ_gbuf_t *circ_gbuf, void *elem);
int __circ_gbuf_pop (circ_gbuf_t *circ_gbuf, void *elem, int read_only);
int __circ_gbuf_free_space(circ_gbuf_t *circ_gbuf);
/* -------------------------------------------------------------------------- */

/**
 * Description:
 *   Zero slots in circular buffer after a pop.
 */
// #define CRICBUF_CLEAN_ON_POP

/**
 * Description:
 *   Defines a global circular buffer `buf` of a given type and size. The type
 *   can be native data types or user-defined data types.
 *
 * Usage:
 *   CIRC_GBUF_DEF(uint8_t, byte_buf, 13);
 *   CIRC_GBUF_DEF(struct foo, foo_buf, 10);
 */
#define CIRC_GBUF_DEF(type, buf, size)            \
    __CIRC_GBUF_V_DEF(type, buf, size)            \
    int buf ## _push_refd(type *pt)               \
    {                                             \
        return __circ_gbuf_push(&buf, pt);        \
    }                                             \
    int buf ## _pop_refd(type *pt)                \
    {                                             \
        return __circ_gbuf_pop(&buf, pt, 0);      \
    }                                             \
    int buf ## _peek_refd(type *pt)               \
    {                                             \
        return __circ_gbuf_pop(&buf, pt, 1);      \
    }

/**
 * Description:
 *   Resets the circular buffer offsets to zero. Does not clean the newly freed
 *   slots.
 */
#define CIRC_GBUF_FLUSH(buf)                      \
    do {                                          \
        buf.push_count = 0;                       \
        buf.pop_count = 0;                        \
    } while(0)

/**
 * Description:
 *   Pushes element pointed to by `elem` at the head of circular buffer `buf`.
 *   This is read-write method, occupancy count increases by one.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Out of space
 */
#define CIRC_GBUF_PUSH(buf, elem)             buf ## _push_refd(elem)

/**
 * Description:
 *   Removes the element at tail from circular buffer `buf` and makes it
 *   available at `elem`. This is read-write method, occupancy count reduces
 *   by one.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Empty
 */
#define CIRC_GBUF_PEEK(buf, elem)             buf ## _peek_refd(elem)

/**
 * Description:
 *   Copies the element at tail of circular buffer `buf` into location pointed
 *   by `elem`. This method is read-only, does not later occupancy status.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Empty
 */
#define CIRC_GBUF_POP(buf, elem)              buf ## _pop_refd(elem)

/**
 * Description:
 *   Returns the number of free slots in the circular buffer `buf`.
 *
 * Returns (int):
 *   0..N - number of slots available.
 */
#define CIRC_GBUF_FS(buf)                     __circ_gbuf_free_space(&buf)

#endif
