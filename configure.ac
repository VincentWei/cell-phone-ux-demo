#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT(cellphone-ux-demo, 1.2.0)
AC_CONFIG_SRCDIR(main.cc)
AC_CONFIG_HEADER(config.h) 

dnl needed for cross-compiling
AC_CANONICAL_SYSTEM

dnl Init automake
AM_INIT_AUTOMAKE

dnl checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

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

case "$with_target" in
  x86)
    CPPFLAGS="$CPPFLAGS -D__ARCH_X86__"
    ;;
  r16)
    CPPFLAGS="$CPPFLAGS -D__ARCH_R16__"
    ;;
  *)
    ;;
esac

AC_ARG_WITH(with_gpu,
[  --with-gpu=[none/gles1/gles2]  GPU support <none/gles1/gles2>])

AC_ARG_WITH(xml2_includes,
[  --with-xml2-includes=DIR  where the LibXML2 includes are])

if test "x$devel_mode" = "xyes"; then
    build_testcase="yes"
fi

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h stdint.h string.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T

dnl Checks for library functions.
AC_CHECK_FUNCS([memset])

dnl ========================================
dnl check for installation of dependencies

PKG_CHECK_MODULES([MINIGUI], [minigui >= 3.2.0])
PKG_CHECK_MODULES([MGPLUS], [mgplus >= 1.4.0])
PKG_CHECK_MODULES([MGEFF], [mgeff >= 1.2.0])
PKG_CHECK_MODULES([MGNCS], [mgncs >= 1.2.0])
PKG_CHECK_MODULES([MGNCS4TOUCH], [mgncs4touch >= 1.2.0])

DEP_LIBS="$DEP_LIBS $MINIGUI_LIBS"
DEP_LIBS="$DEP_LIBS $MGPLUS_LIBS"
DEP_LIBS="$DEP_LIBS $MGEFF_LIBS"
DEP_LIBS="$DEP_LIBS $MGNCS_LIBS"
DEP_LIBS="$DEP_LIBS $MGNCS4TOUCH_LIBS"

AC_CHECK_LIB(sqlite3, sqlite3_open_v2, DEP_LIBS="$DEP_LIBS -lsqlite3", sqlite3_installed=no)
if test "x$sqlite3_installed" = "xno"; then
    AC_MSG_ERROR([[SQLite3 is not installed! Run "sudo apt install libsqlite3-dev" to install it if you are using Ubuntu.]])
fi

AC_CHECK_LIB(xml2, xmlFree, DEP_LIBS="$DEP_LIBS -lxml2", xml2_installed=no)
if test "x$xml2_installed" = "xno"; then
    AC_MSG_ERROR([[LibXML2 is not installed! Run "sudo apt install libxml2-dev" to install it if you are using Ubuntu.]])
else
    if test "x$with_xml2_includes" = "x"; then
        XML2_INC_DIR="-I/usr/include/libxml2"
    else
        XML2_INC_DIR="-I$with_xml2_includes"
    fi
fi
CPPFLAGS="$CPPFLAGS $XML2_INC_DIR"

AC_CHECK_LIB(chipmunk, cpInitChipmunk, DEP_LIBS="$DEP_LIBS -lchipmunk", chipmunk_installed=no, "-lm")
if test "x$chipmunk_installed" = "xno"; then
    AC_MSG_ERROR([[Chipmunk V5.3.x is not installed! Please make and install it from 3rd-party/ directory.]])
fi

if test "x$devel_mode" = "xyes"; then
    CPPFLAGS="$CPPFLAGS -DDEBUG -D_DEBUG -DPHYANIM_DEBUG -DENABLE_ANIM_FPS_TEST"
    dnl AC_DEFINE(PHYANIM_DEBUG, 1, [Define if debug ScrollView enabled])
    dnl AC_DEFINE(ENABLE_ANIM_FPS_TEST, 1, [Define if animation FPS test enabled])
    if test "$ac_cv_prog_gcc" = "yes"; then
        CPPFLAGS="$CPPFLAGS -Werror"
    fi
else
    CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

if test "$ac_cv_prog_gcc" = "yes"; then
    CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE -Wall -pipe"
    CFLAGS="$CFLAGS -Wstrict-prototypes"
fi

case "$with_gpu" in
  gles1)
    CPPFLAGS="$CPPFLAGS -DGPU_GLES1"
    dnl AC_DEFINE(GPU_GLES1, 1, [Define if use OpenGL ES V1.1])
    ;;
  gles2)
    CPPFLAGS="$CPPFLAGS -DGPU_GLES2"
    dnl AC_DEFINE(GPU_GLES2, 1, [Define if use OpenGL ES V2.0])
    ;;
  *)
    CPPFLAGS="$CPPFLAGS -DGPU_NONE"
    dnl AC_DEFINE(GPU_NONE, 1, [Define if do not use GPU])
    ;;
esac

CPPFLAGS="$CPPFLAGS -DENABLE_SCROLLVIEW"
dnl AC_DEFINE(ENABLE_SCROLLVIEW, 1, [Define if ScrollView enabled])

AC_SUBST(DEP_LIBS)

AC_OUTPUT(
    Makefile
    m4/Makefile
    etc/Makefile
    include/Makefile
    common/Makefile
    controls/Makefile
    ncs4pad/Makefile
    ncs4pad/include/Makefile
    ncs4pad/include/misc/Makefile
    ncs4pad/include/pieces/Makefile
    ncs4pad/include/physics-animation/Makefile
    ncs4pad/src/Makefile
    ncs4pad/src/misc/Makefile
    ncs4pad/src/pieces/Makefile
    ncs4pad/src/physics-animation/Makefile
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
    res/Makefile
    res/address-book/Makefile
    res/camera/Makefile
    res/common/Makefile
    res/desktop/Makefile
    res/dial/Makefile
    res/flight-gl/Makefile
    res/font/Makefile
    res/gvfb/Makefile
    res/lockscreen/Makefile
    res/musicplayer/Makefile
    res/musicplayer/Album/Makefile
    res/musicplayer/player/Makefile
    res/phone-call/Makefile
    res/pictureviewer/Makefile
    res/sample-activity/Makefile
)

