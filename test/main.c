/*
 * Copyright (c) Vicent Marti. All rights reserved.
 *
 * This file is part of clar, distributed under the ISC license.
 * For full terms see the included COPYING file.
 */

#include "clar.h"

/*
 * Sample main() for clar tests.
 *
 * You should write your own main routine for clar tests that does specific
 * setup and teardown as necessary for your application.  The only required
 * line is the call to `clar_test(argc, argv)`, which will execute the test
 * suite.  If you want to check the return value of the test application,
 * your main() should return the same value returned by clar_test().
 */

int main(int argc, char *argv[])
{
	int ret;

	ret = clar_test(argc, argv);

	return ret;
}
