#ifndef INCLUDE_buffer_ext_h__
#define INCLUDE_buffer_ext_h__

#include "buffer.h"

void gh_buf_consume(gh_buf *buf, const char *end);
void gh_buf_truncate(gh_buf *buf, size_t len);
void gh_buf_rtruncate_at_char(gh_buf *path, char separator);

int gh_buf_join_n(gh_buf *buf, char separator, int nbuf, ...);
int gh_buf_join(gh_buf *buf, char separator, const char *str_a, const char *str_b);

/**
 * Join two strings as paths, inserting a slash between as needed.
 * @return 0 on success, -1 on failure
 */
static inline int gh_buf_joinpath(gh_buf *buf, const char *a, const char *b)
{
	return gh_buf_join(buf, '/', a, b);
}

static inline ssize_t gh_buf_rfind_next(gh_buf *buf, char ch)
{
	ssize_t idx = (ssize_t)buf->size - 1;
	while (idx >= 0 && buf->ptr[idx] == ch) idx--;
	while (idx >= 0 && buf->ptr[idx] != ch) idx--;
	return idx;
}

static inline ssize_t gh_buf_rfind(gh_buf *buf, char ch)
{
	ssize_t idx = (ssize_t)buf->size - 1;
	while (idx >= 0 && buf->ptr[idx] != ch) idx--;
	return idx;
}

static inline ssize_t gh_buf_find(gh_buf *buf, char ch)
{
	size_t idx = 0;
	while (idx < buf->size && buf->ptr[idx] != ch) idx++;
	return (idx == buf->size) ? -1 : (ssize_t)idx;
}

/* Remove whitespace from the end of the buffer */
void gh_buf_rtrim(gh_buf *buf);

/* Write data as base64 encoded in buffer */
int gh_buf_put_base64(gh_buf *buf, const char *data, size_t len);

/*
 * Insert, remove or replace a portion of the buffer.
 *
 * @param buf The buffer to work with
 *
 * @param where The location in the buffer where the transformation
 * should be applied.
 *
 * @param nb_to_remove The number of chars to be removed. 0 to not
 * remove any character in the buffer.
 *
 * @param data A pointer to the data which should be inserted.
 *
 * @param nb_to_insert The number of chars to be inserted. 0 to not
 * insert any character from the buffer.
 *
 * @return 0 or an error code.
 */
int gh_buf_splice(
	gh_buf *buf,
	size_t where,
	size_t nb_to_remove,
	const char *data,
	size_t nb_to_insert);

/* Drain a stream into a buffer
 * @return 0 on success
 */
int gh_buf_read_from_stream(gh_buf *buf, int stream);

/* Write a buffer to a stream
 * @return 0 on success
 */
int gh_buf_write_to_stream(const gh_buf *buf, int stream);

#endif
