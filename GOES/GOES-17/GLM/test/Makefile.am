# This is an automake file for the Geostationary Lightning Mapper
# (GLM) test code.

# Ed Hartnett 10/10/2019
# Amsterdam

# Un comment to use a more verbose test driver
SH_LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose
LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose

TEST_EXTENSIONS = .sh

# Find necessary include files.
AM_CPPFLAGS = -I$(top_srcdir)/GOES/GOES-17/GLM/src

# Link to our assembled library.
AM_LDFLAGS = ${top_builddir}/GOES/GOES-17/GLM/src/libgoesglm.la
LDADD = ${top_builddir}/GOES/GOES-17/GLM/src/libgoesglm.la

# Build our test program.
check_PROGRAMS = tst_glm_read tst_glm_read_arrays tst_glm_ncgen_write

# Source code for the tests.
tst_glm_read_SOURCES = tst_glm_read.c un_test.h tst_utils.c
tst_glm_read_arrays_SOURCES = tst_glm_read_arrays.c un_test.h tst_utils.c

# Run our test program.
TESTS = run_glm_tests.sh 

# Make sure this file is included in distribution.
EXTRA_DIST = run_glm_tests.sh

# Clean data files for distclean target.
DISTCLEANFILES = *.nc
