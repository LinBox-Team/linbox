# Check for GMP
# Bradford Hovinen, 2001-06-13
# Inspired by gnome-bonobo-check.m4 by Miguel de Icaza, 99-04-12
# Stolen from Chris Lahey       99-2-5
# stolen from Manish Singh again
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl LB_CHECK_GMP ([MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl Test for the GNU Multiprecision library and define GMP_CFLAGS and GMP_LIBS

AC_DEFUN([LB_CHECK_GMP],
[

AC_ARG_WITH(gmp-prefix,[  --with-gmp-prefix=PFX      Prefix where GMP is installed (optional)],
[gmp_prefix="$withval"],[gmp_prefix=""])

AC_ARG_WITH(gcc-prefix,[  --with-gcc-prefix=PFX      Prefix where gcc is installed (optional)],
[gcc_prefix="$withval"],[gcc_prefix=""])

min_gmp_version=ifelse([$1], ,3.1.1,$1)

AC_MSG_CHECKING(for GMP >= $min_gmp_version)

if test x$gmp_prefix != x; then
	LD_LIBRARY_PATH=$gmp_prefix/lib:$LD_LIBRARY_PATH
	export LD_LIBRARY_PATH
	CPLUS_INCLUDE_PATH=$gmp_prefix/include:$CPLUS_INCLUDE_PATH
	export CPLUS_INCLUDE_PATH

	GMP_CFLAGS="-I$(gmp_prefix)/include"
	# FIXME use of -R below ) should be replaced with portable version.
	# FIXME remove use of gcc_prefix.  It has to do with a prob in udel setup. 
	# Fix that locally, then remove else clause here.
        if test x$gcc_prefix = x; then
	  GMP_LIBS="-L$(gmp_prefix)/lib -R$(gmp_prefix)/lib -lgmp"
	else
	  GMP_LIBS="-R$(gcc_prefix)/lib -L$(gmp_prefix)/lib -R$(gmp_prefix)/lib -lgmp"
	fi

	AC_SUBST(gmp_prefix)
	AC_SUBST(gcc_prefix)
	AC_SUBST(GMP_CFLAGS)
	AC_SUBST(GMP_LIBS)
	AC_DEFINE(HAVE_GMP)
	AC_MSG_RESULT(found)
fi

GMP_CFLAGS="-I$gmp_prefix/include"
GMP_LIBS="-L$gmp_prefix/lib -lgmp"
AC_SUBST(gmp_prefix)
AC_SUBST(GMP_CFLAGS)
AC_SUBST(GMP_LIBS)
AC_DEFINE(HAVE_GMP)
echo GMP IS AT ............. $gmp_prefix
dnl Check for existence

AC_CHECK_LIB(gmp, __gmpz_init,
[
dnl Check if the version is new enough
dnl FIXME

ifelse([$2], , :, [$2])
AC_MSG_RESULT(found)
],
[
AC_MSG_RESULT(not found)
ifelse([$3], , :, [$3])
])

])
