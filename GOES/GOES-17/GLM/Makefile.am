# This is an automake file for the Using NetCDF sample code.

# Ed Hartnett 10/10/2019
# Amsterdam

# Un comment to use a more verbose test driver
SH_LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose
LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose

TEST_EXTENSIONS = .sh

# This is a libtool library.
lib_LTLIBRARIES = libgoesglm.la
libgoesglm_la_LDFLAGS = -version-info 0:0:0
libgoesglm_la_SOURCES = glm_read.c goes_glm.h

# Build our test program.
check_PROGRAMS = tst_glm_read

# Source code for the test.
tst_glm_read_SOURCES = tst_glm_read.c un_test.h tst_utils.c
tst_glm_read_LDADD = libgoesglm.la

# Run our test program.
TESTS = tst_glm.sh 

# Make sure this file is included in distribution.
EXTRA_DIST = tst_glm.sh

# Clean data files for distclean target.
DISTCLEANFILES = *.nc
