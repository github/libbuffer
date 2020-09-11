#include <string.h>
#include <github/buffer.h>
#include "clar.h"

static const char *test_string = "Have you seen that? Have you seeeen that??";

void test_buf_basic__resize(void)
{
	gh_buf buf1 = GH_BUF_INIT;
	gh_buf_puts(&buf1, test_string);
	cl_assert(gh_buf_oom(&buf1) == 0);
	cl_assert_equal_s(gh_buf_cstr(&buf1), test_string);

	gh_buf_puts(&buf1, test_string);
	cl_assert(strlen(gh_buf_cstr(&buf1)) == strlen(test_string) * 2);
	gh_buf_free(&buf1);
}

void test_buf_basic__printf(void)
{
	gh_buf buf2 = GH_BUF_INIT;
	gh_buf_printf(&buf2, "%s %s %d ", "shoop", "da", 23);
	cl_assert(gh_buf_oom(&buf2) == 0);
	cl_assert_equal_s(gh_buf_cstr(&buf2), "shoop da 23 ");

	gh_buf_printf(&buf2, "%s %d", "woop", 42);
	cl_assert(gh_buf_oom(&buf2) == 0);
	cl_assert_equal_s(gh_buf_cstr(&buf2), "shoop da 23 woop 42");
	gh_buf_free(&buf2);
}
