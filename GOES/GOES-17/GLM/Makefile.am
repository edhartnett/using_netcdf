# This is an automake file for the Using NetCDF sample code.

# Ed Hartnett 10/10/2019
# Amsterdam

# Un comment to use a more verbose test driver
SH_LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose
LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver-verbose

TEST_EXTENSIONS = .sh

# Build our test program.
check_PROGRAMS = glm_read

# Run our test program.
TESTS = tst_glm.sh 

# Make sure this file is included in distribution.
EXTRA_DIST = tst_glm.sh

# Clean data files for distclean target.
DIST_CLEAN = *.nc
