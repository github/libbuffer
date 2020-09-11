#include <string.h>
#include <github/buffer.h>
#include <github/buffer_ext.h>
#include "clar.h"

static gh_buf _buf;

void test_buf_splice__initialize(void) {
   gh_buf_init(&_buf, 16);
}

void test_buf_splice__cleanup(void) {
   gh_buf_free(&_buf);
}

void test_buf_splice__preprend(void)
{
	gh_buf_sets(&_buf, "world!");

	cl_must_pass(gh_buf_splice(&_buf, 0, 0, "Hello Dolly", strlen("Hello ")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__append(void)
{
	gh_buf_sets(&_buf, "Hello");

	cl_must_pass(gh_buf_splice(&_buf, gh_buf_len(&_buf), 0, " world!", strlen(" world!")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__insert_at(void)
{
	gh_buf_sets(&_buf, "Hell world!");

	cl_must_pass(gh_buf_splice(&_buf, strlen("Hell"), 0, "o", strlen("o")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__remove_at(void)
{
	gh_buf_sets(&_buf, "Hello world of warcraft!");

	cl_must_pass(gh_buf_splice(&_buf, strlen("Hello world"), strlen(" of warcraft"), "", 0));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__replace(void)
{
	gh_buf_sets(&_buf, "Hell0 w0rld!");

	cl_must_pass(gh_buf_splice(&_buf, strlen("Hell"), strlen("0 w0"), "o wo", strlen("o wo")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__replace_with_longer(void)
{
	gh_buf_sets(&_buf, "Hello you!");

	cl_must_pass(gh_buf_splice(&_buf, strlen("Hello "), strlen("you"), "world", strlen("world")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__replace_with_shorter(void)
{
	gh_buf_sets(&_buf, "Brave new world!");

	cl_must_pass(gh_buf_splice(&_buf, 0, strlen("Brave new"), "Hello", strlen("Hello")));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__truncate(void)
{
	gh_buf_sets(&_buf, "Hello world!!");

	cl_must_pass(gh_buf_splice(&_buf, strlen("Hello world!"), strlen("!"), "", 0));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}

void test_buf_splice__dont_do_anything(void)
{
	gh_buf_sets(&_buf, "Hello world!");

	cl_must_pass(gh_buf_splice(&_buf, 3, 0, "Hello", 0));

	cl_assert_equal_s("Hello world!", gh_buf_cstr(&_buf));
}
