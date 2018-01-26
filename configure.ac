#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT(cellphone-ux-demo, 1.2.0)
AC_CONFIG_SRCDIR(main.cc)

dnl needed for cross-compiling
AC_CANONICAL_SYSTEM

dnl checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h) 

#user define macores
devel_mode="no"

with_target="x86"
with_gpu="none"

#Check user define macores.

AC_ARG_ENABLE(develmode,
[  --enable-develmode       developer mode <default=no>],
devel_mode=$enableval)

AC_ARG_WITH(with_target,
[  --with-target=[x86/r16]  target <x86/r16>])

AC_ARG_WITH(with_gpu,
[  --with-gpu=[none/gles1/gles2]  GPU support <none/gles1/gles2>])

if test "x$devel_mode" = "xyes"; then
    build_testcase="yes"
    build_anim_fps_test="yes"
fi

#user define macores
if test "x$build_anim_fps_test" = "xyes" ; then
    CPPFLAGS="$CPPFLAGS -DENABLE_ANIM_FPS_TEST"
fi

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h stdint.h string.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T

# Checks for library functions.
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

dnl ========================================
dnl check for installation of dependencies

PKG_CHECK_MODULES([MINIGUI], [minigui >= 3.2.0])
PKG_CHECK_MODULES([MGPLUS], [mgplus >= 1.4.0])
PKG_CHECK_MODULES([MGEFF], [mgeff >= 1.2.0])
PKG_CHECK_MODULES([MGNCS], [mgncs >= 1.2.0])

PKG_CHECK_MODULES((SQLITE3], [sqlite3 >= 3.7.4])
PKG_CHECK_MODULES((XML2], [xml2 >= 2.2.0])

DEP_LIBS="$DEP_LIBS $MINIGUI_LIBS"
DEP_LIBS="$DEP_LIBS $MGUTILS_LIBS"
DEP_LIBS="$DEP_LIBS $MGPLUS_LIBS"
DEP_LIBS="$DEP_LIBS $MGEFF_LIBS"
DEP_LIBS="$DEP_LIBS $MGNCS_LIBS"
DEP_LIBS="$DEP_LIBS $MGNCS4TOUCH_LIBS"

AC_CHECK_LIB(sqlite3, sqlite3_open_v2, DEP_LIBS="$DEP_LIBS -lsqlite3", sqlite3_installed=no)
AC_CHECK_LIB(xml2, xmlFree, DEP_LIBS="$DEP_LIBS -lxml2", xml2_installed=no)

if test "x$devel_mode" = "xyes"; then
    if test "$ac_cv_prog_gcc" = "yes"; then
        CPPFLAGS="$CPPFLAGS -Wall -Werror"
    fi
fi

if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -D_GNU_SOURCE -Wstrict-prototypes -pipe"
fi

AC_OUTPUT(
    Makefile
    m4/Makefile
    include/Makefile
    common/Makefile
    controls/Makefile
    ncs4pad/Makefile
    ncs4pad/include/Makefile
    ncs4pad/src/Makefile
    apps/Makefile
    apps/address-book/Makefile
    apps/calc/Makefile
    apps/callhistory/Makefile
    apps/camera/Makefile
    apps/common/Makefile
    apps/coverflow-gl/Makefile
    apps/dial/Makefile
    apps/eraser/Makefile
    apps/flight-gl/Makefile
    apps/music/Makefile
    apps/phone-call/Makefile
    apps/photo/Makefile
    apps/sample-activity/Makefile
    apps/sample-navigation/Makefile
    apps/screen-lock/Makefile
    apps/sms/Makefile
    apps/sys-setting/Makefile
    apps/time-setting/Makefile
    desktop/Makefile
    inner-res/Makefile
)

