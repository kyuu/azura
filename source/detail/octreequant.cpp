/*
    The MIT License (MIT)

    Copyright (c) 2013-2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

// This module uses parts from http://rosettacode.org/wiki/Color_quantization/C
// TODO: Rewrite and drop those parts, because of possible incompatibilities with the GFDL.

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cmath>

#define ON_INHEAP   1

#include "../types.hpp"
#include "octreequant.hpp"


namespace azura {

    struct oct_node {
        u32 r;
        u32 g;
        u32 b; /* sum of all child node colors */
        u32 count;
        u32 heap_idx;
        u32 children_count;
        u32 child_idx;
        u32 flags;
        u32 depth;
        oct_node* parent;
        oct_node* children[8];
    };

    struct node_heap {
        int alloc;
        int n;
        oct_node** buf;
    };

    inline int cmp_node(oct_node* a, oct_node* b)
    {
        if (a->children_count < b->children_count) {
            return -1;
        }

        if (a->children_count > b->children_count) {
            return 1;
        }

        int ac = a->count >> a->depth;
        int bc = b->count >> b->depth;

        return ac < bc ? -1 : ac > bc;
    }

    void down_heap(node_heap* h, oct_node* p)
    {
        int n = p->heap_idx;
        int m;

        while (true) {
            m = n * 2;

            if (m >= h->n) {
                break;
            }

            if (m + 1 < h->n && cmp_node(h->buf[m], h->buf[m + 1]) > 0) {
                m++;
            }

            if (cmp_node(p, h->buf[m]) <= 0) {
                break;
            }

            h->buf[n] = h->buf[m];
            h->buf[n]->heap_idx = n;
            n = m;
        }

        h->buf[n] = p;
        p->heap_idx = n;
    }

    void up_heap(node_heap* h, oct_node* p)
    {
        int n = p->heap_idx;
        oct_node* prev;

        while (n > 1) {
            prev = h->buf[n / 2];

            if (cmp_node(p, prev) >= 0) {
                break;
            }

            h->buf[n] = prev;
            prev->heap_idx = n;
            n /= 2;
        }

        h->buf[n] = p;
        p->heap_idx = n;
    }

    void heap_add(node_heap *h, oct_node* p)
    {
        if ((p->flags & ON_INHEAP)) {
            down_heap(h, p);
            up_heap(h, p);
            return;
        }

        p->flags |= ON_INHEAP;

        if (!h->n) {
            h->n = 1;
        }

        if (h->n >= h->alloc) {
            while (h->n >= h->alloc) h->alloc += 1024;
            h->buf = (oct_node**)realloc(h->buf, sizeof(oct_node*) * h->alloc);
        }

        p->heap_idx = h->n;
        h->buf[h->n++] = p;
        up_heap(h, p);
    }

    oct_node* pop_heap(node_heap *h)
    {
        if (h->n <= 1) {
            return 0;
        }

        oct_node* ret = h->buf[1];
        h->buf[1] = h->buf[--h->n];

        h->buf[h->n] = 0;

        h->buf[1]->heap_idx = 1;
        down_heap(h, h->buf[1]);

        return ret;
    }

    static oct_node* pool = 0;
    static int len = 0;
    oct_node* node_new(u8 idx, u8 depth, oct_node* p)
    {
        if (len <= 1) {
            oct_node* p = (oct_node*)calloc(sizeof(oct_node), 2048);
            p->parent = pool;
            pool = p;
            len = 2047;
        }

        oct_node* x = pool + len--;

        x->child_idx = idx;
        x->depth = depth;
        x->parent = p;

        if (p) {
            p->children_count++;
        }

        return x;
    }

    void node_free()
    {
        oct_node* p;
        while (pool) {
            p = pool->parent;
            free(pool);
            pool = p;
        }

        len = 0;
    }

    oct_node* node_insert(oct_node* root, u8 *pix)
    {
        u8 depth = 0;
        for (u8 bit = 1 << 7; ++depth < 8; bit >>= 1) {
            u8 i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
            if (!root->children[i]) {
                root->children[i] = node_new(i, depth, root);
            }
            root = root->children[i];
        }

        root->r += pix[0];
        root->g += pix[1];
        root->b += pix[2];
        root->count++;

        return root;
    }

    oct_node* node_fold(oct_node* p)
    {
        assert(p->children_count == 0);

        oct_node* q = p->parent;

        q->count += p->count;

        q->r += p->r;
        q->g += p->g;
        q->b += p->b;

        q->children_count --;
        q->children[p->child_idx] = 0;

        return q;
    }

    void color_replace(oct_node* root, u8* src, u8* dst)
    {
        for (u8 bit = 1 << 7; bit; bit >>= 1) {
            u8 i = !!(src[1] & bit) * 4 + !!(src[0] & bit) * 2 + !!(src[2] & bit);
            if (!root->children[i]) break;
            root = root->children[i];
        }

        *dst = root->heap_idx - 1;
    }

    void OctreeQuant(RGB* src_pixels, int pixels_count, u8* dst_pixels, RGB dst_palette[256])
    {
        node_heap heap = { 0, 0, 0 };
        oct_node* root = node_new(0, 0, 0);

        RGB* pix = src_pixels;
        for (int i = 0; i < pixels_count; i++) {
            heap_add(&heap, node_insert(root, (u8*)pix));
            pix++;
        }

        while (heap.n > 256 /* palette size */ + 1) {
            heap_add(&heap, node_fold(pop_heap(&heap)));
        }

        for (int i = 1; i < heap.n; i++) {
            oct_node* node = heap.buf[i];

            double c = node->count;

            node->r = (u32)(node->r / c + .5);
            node->g = (u32)(node->g / c + .5);
            node->b = (u32)(node->b / c + .5);

            RGB* plt_entry = dst_palette + (i - 1);

            plt_entry->red   = node->r;
            plt_entry->green = node->g;
            plt_entry->blue  = node->b;
        }

        RGB* sptr = src_pixels;
        u8* dptr = dst_pixels;
        for (int i = 0; i < pixels_count; i++) {
            color_replace(root, (u8*)sptr, dptr);
            sptr++;
            dptr++;
        }

        node_free();
        free(heap.buf);
    }

}
