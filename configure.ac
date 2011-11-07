AC_PREREQ(2.59)

AC_INIT(cutter-mock, 0.0.1, y.hayamizu@gmail.com)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile
	src/Makefile
	test/Makefile])

m4_ifdef([AC_CHECK_GCUTTER],
         [AC_CHECK_GCUTTER],
         [ac_cv_cutter_available="no"])

if test "$ac_cv_cutter_available" = "no"; then
  AC_MSG_ERROR([Cutter is required.])
fi

dnl **************************************************************
dnl Check for GLib
dnl **************************************************************

PKG_CHECK_MODULES([GLIB],
                  [glib-2.0 gthread-2.0],
                  [ac_glib_2_0_available="yes"],
                  [ac_glib_2_0_available="no"])

if test "$ac_glib_2_0_available" = "no"; then
  AC_MSG_ERROR([GLib is required.])
fi
AM_CONDITIONAL([WITH_GLIB], [test "$ac_glib_2_0_available" = "yes"])

AC_OUTPUT

echo
echo "Configure Result:"
echo
echo "  CPU arch: $ac_arch"
echo
echo "== required headers and libraries =="
echo "  Cutter : $CUTTER"
echo "    GLib : $ac_glib_2_0_available"
echo
