# This is the main configure file for the Using NetCDF sample code.

# Ed Hartnett 10/10/2019
# Amsterdam

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([using-netcdf], [1.0-development], [])

# Find out about the host we're building on.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

# Find out about the target we're building for.
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# Find the C compiler.
AC_PROG_CC
AM_PROG_CC_C_O
AC_C_CONST
AC_PROG_CPP

# Find the Fortran compiler.
AC_PROG_FC
AC_PROG_F77

# Find the install program.
AC_PROG_INSTALL

# Check to see if any macros must be set to enable large (>2GB) files.
AC_SYS_LARGEFILE

# Check for netCDF C library.
AC_SEARCH_LIBS([nc_create], [netcdf], [],
                            [AC_MSG_ERROR([Can't find or link to the netcdf C library, set CPPFLAGS/LDFLAGS.])])

# Check for netCDF Fortran library.
dnl AC_LANG_PUSH(Fortran)
dnl AC_SEARCH_LIBS([nf_create], [netcdff], [],
dnl                             [AC_MSG_ERROR([Can't find or link to the netcdf Fortran library, set CPPFLAGS/LDFLAGS.])])
dnl AC_LANG_POP(Fortran)

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
	GOES/Makefile
	GOES/GOES-17/Makefile
	GOES/GOES-17/GLM/Makefile
	GOES/GOES-17/GLM/src/Makefile
	GOES/GOES-17/GLM/test/Makefile
        ])
AC_OUTPUT()
