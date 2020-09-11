#
# Copyright (c) Vicent Marti. All rights reserved.
#
# This file is part of clar, distributed under the ISC license.
# For full terms see the included COPYING file.
#

#
# Set up the path to the clar sources and to the fixtures directory
#
# The fixture path needs to be an absolute path so it can be used
# even after we have chdir'ed into the test directory while testing.
#
CURRENT_MAKEFILE  := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
TEST_DIRECTORY    := $(abspath $(dir $(CURRENT_MAKEFILE)))
CLAR_PATH         := $(TEST_DIRECTORY)/
CLAR_CASES_PATH   := $(TEST_DIRECTORY)/cases/
CLAR_FIXTURE_PATH := $(TEST_DIRECTORY)/resources/

CFLAGS=-g -I.. -I. -I../include -I$(CLAR_CASES_PATH) -Wall -DCLAR_FIXTURE_PATH=\"$(CLAR_FIXTURE_PATH)\"
LDFLAGS=-L..
# list the objects that go into our test
objects  = main.o $(patsubst %.c, %.o, $(wildcard cases/*.c)) $(patsubst %.c, %.o, $(wildcard cases/**/*.c))
# build the test executable itself
clar_test: $(objects) $(CLAR_CASES_PATH)clar.suite
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ "$(CLAR_PATH)clar.c" $(objects) -lbuffer

# build the clar.suite file of test metadata
$(CLAR_CASES_PATH)clar.suite: $(objects)
	python3 "$(CLAR_PATH)generate.py" $(CLAR_CASES_PATH)

# remove all generated files
clean:
	$(RM) -rf *.o cases/*.o cases/**/*.o $(CLAR_CASES_PATH)clar.suite .clarcache clar_test clar_test.dSYM
