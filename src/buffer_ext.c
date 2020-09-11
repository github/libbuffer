/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>
#include <errno.h>

#include "github/buffer.h"
#include "github/buffer_ext.h"

static const char b64str[64] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int gh_buf_put_base64(gh_buf *buf, const char *data, size_t len)
{
	size_t extra = len % 3;
	unsigned char *write, a, b, c;
	const unsigned char *read = (const unsigned char *)data;

	if (gh_buf_grow(buf, buf->size + 4 * ((len / 3) + !!extra) + 1) < 0)
		return -1;

	write = (unsigned char *)&buf->ptr[buf->size];

	/* convert each run of 3 bytes into 4 output bytes */
	for (len -= extra; len > 0; len -= 3) {
		a = *read++;
		b = *read++;
		c = *read++;

		*write++ = b64str[a >> 2];
		*write++ = b64str[(a & 0x03) << 4 | b >> 4];
		*write++ = b64str[(b & 0x0f) << 2 | c >> 6];
		*write++ = b64str[c & 0x3f];
	}

	if (extra > 0) {
		a = *read++;
		b = (extra > 1) ? *read++ : 0;

		*write++ = b64str[a >> 2];
		*write++ = b64str[(a & 0x03) << 4 | b >> 4];
		*write++ = (extra > 1) ? b64str[(b & 0x0f) << 2] : '=';
		*write++ = '=';
	}

	buf->size = ((char *)write) - buf->ptr;
	buf->ptr[buf->size] = '\0';

	return 0;
}

void gh_buf_consume(gh_buf *buf, const char *end)
{
	if (end > buf->ptr && end <= buf->ptr + buf->size) {
		size_t consumed = end - buf->ptr;
		memmove(buf->ptr, end, buf->size - consumed);
		buf->size -= consumed;
		buf->ptr[buf->size] = '\0';
	}
}

void gh_buf_truncate(gh_buf *buf, size_t len)
{
	if (len < buf->size) {
		buf->size = len;
		buf->ptr[buf->size] = '\0';
	}
}

void gh_buf_rtruncate_at_char(gh_buf *buf, char separator)
{
	ssize_t idx = gh_buf_rfind_next(buf, separator);
	gh_buf_truncate(buf, idx < 0 ? 0 : (size_t)idx);
}

int gh_buf_join_n(gh_buf *buf, char separator, int nbuf, ...)
{
	va_list ap;
	int i;
	size_t total_size = 0, original_size = buf->size;
	char *out, *original = buf->ptr;

	if (buf->size > 0 && buf->ptr[buf->size - 1] != separator)
		++total_size; /* space for initial separator */

	/* Make two passes to avoid multiple reallocation */

	va_start(ap, nbuf);
	for (i = 0; i < nbuf; ++i) {
		const char* segment;
		size_t segment_len;

		segment = va_arg(ap, const char *);
		if (!segment)
			continue;

		segment_len = strlen(segment);
		total_size += segment_len;
		if (segment_len == 0 || segment[segment_len - 1] != separator)
			++total_size; /* space for separator */
	}
	va_end(ap);

	/* expand buffer if needed */
	if (total_size == 0)
		return 0;
	if (gh_buf_grow(buf, buf->size + total_size + 1) < 0)
		return -1;

	out = buf->ptr + buf->size;

	/* append separator to existing buf if needed */
	if (buf->size > 0 && out[-1] != separator)
		*out++ = separator;

	va_start(ap, nbuf);
	for (i = 0; i < nbuf; ++i) {
		const char* segment;
		size_t segment_len;

		segment = va_arg(ap, const char *);
		if (!segment)
			continue;

		/* deal with join that references buffer's original content */
		if (segment >= original && segment < original + original_size) {
			size_t offset = (segment - original);
			segment = buf->ptr + offset;
			segment_len = original_size - offset;
		} else {
			segment_len = strlen(segment);
		}

		/* skip leading separators */
		if (out > buf->ptr && out[-1] == separator)
			while (segment_len > 0 && *segment == separator) {
				segment++;
				segment_len--;
			}

		/* copy over next buffer */
		if (segment_len > 0) {
			memmove(out, segment, segment_len);
			out += segment_len;
		}

		/* append trailing separator (except for last item) */
		if (i < nbuf - 1 && out > buf->ptr && out[-1] != separator)
			*out++ = separator;
	}
	va_end(ap);

	/* set size based on num characters actually written */
	buf->size = out - buf->ptr;
	buf->ptr[buf->size] = '\0';

	return 0;
}

int gh_buf_join(
	gh_buf *buf,
	char separator,
	const char *str_a,
	const char *str_b)
{
	size_t strlen_a = str_a ? strlen(str_a) : 0;
	size_t strlen_b = strlen(str_b);
	int need_sep = 0;
	ssize_t offset_a = -1;

	/* not safe to have str_b point internally to the buffer */
	assert(str_b < buf->ptr || str_b >= buf->ptr + buf->size);

	/* figure out if we need to insert a separator */
	if (separator && strlen_a) {
		while (*str_b == separator) { str_b++; strlen_b--; }
		if (str_a[strlen_a - 1] != separator)
			need_sep = 1;
	}

	/* str_a could be part of the buffer */
	if (str_a >= buf->ptr && str_a < buf->ptr + buf->size)
		offset_a = str_a - buf->ptr;

	if (gh_buf_grow(buf, strlen_a + strlen_b + need_sep + 1) < 0)
		return -1;
	assert(buf->ptr);

	/* fix up internal pointers */
	if (offset_a >= 0)
		str_a = buf->ptr + offset_a;

	/* do the actual copying */
	if (offset_a != 0 && str_a)
		memmove(buf->ptr, str_a, strlen_a);
	if (need_sep)
		buf->ptr[strlen_a] = separator;
	memcpy(buf->ptr + strlen_a + need_sep, str_b, strlen_b);

	buf->size = strlen_a + strlen_b + need_sep;
	buf->ptr[buf->size] = '\0';

	return 0;
}

void gh_buf_rtrim(gh_buf *buf)
{
	while (buf->size > 0) {
		if (!isspace(buf->ptr[buf->size - 1]))
			break;

		buf->size--;
	}

	buf->ptr[buf->size] = '\0';
}

int gh_buf_splice(
	gh_buf *buf,
	size_t where,
	size_t nb_to_remove,
	const char *data,
	size_t nb_to_insert)
{
	assert(buf &&
		where <= gh_buf_len(buf) &&
		where + nb_to_remove <= gh_buf_len(buf));

	/* Ported from git.git
	 * https://github.com/git/git/blob/16eed7c/strbuf.c#L159-176
	 */
	if (gh_buf_grow(buf, gh_buf_len(buf) + nb_to_insert - nb_to_remove) < 0)
		return -1;

	memmove(buf->ptr + where + nb_to_insert,
			buf->ptr + where + nb_to_remove,
			buf->size - where - nb_to_remove);

	memcpy(buf->ptr + where, data, nb_to_insert);

	buf->size = buf->size + nb_to_insert - nb_to_remove;
	buf->ptr[buf->size] = '\0';
	return 0;
}

int gh_buf_read_from_stream(gh_buf *buf, int stream)
{
        for (;;) {
                ssize_t cnt;

                gh_buf_grow(buf, buf->size + 8192);

                cnt = read(stream, buf->ptr + buf->size, buf->asize - buf->size - 1);
                if ((cnt < 0) && (errno == EAGAIN || errno == EINTR)) {
                        continue;
                } else if (cnt < 0) {
                        return -1;
                } else if (cnt == 0) {
                        return 0;
                } else {
                        gh_buf_set_len(buf, buf->size + cnt);
                }
        }

        return -1;      /* NOT REACHED */
}

int gh_buf_write_to_stream(const gh_buf *buf, int stream)
{
        size_t offset=0;

        while (offset < buf->size) {
                ssize_t sent;

                sent = write(stream, buf->ptr + offset, buf->size - offset);
                if ((sent < 0) && (errno == EAGAIN || errno == EINTR)) {
                        continue;
                } else if (sent < 0) {
                        return -1;
                } else {
                        offset += sent;
                }
        }

        return -1;      /* NOT REACHED */
}
