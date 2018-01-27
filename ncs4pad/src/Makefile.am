AM_CPPFLAGS = -I../../include -I../include -I../include/misc -I../include/physics-animation -I../include/pieces

SUBDIRS = misc physics-animation pieces

noinst_LTLIBRARIES = libncs4pad.la

libncs4pad_la_SOURCES = \
    mcenterhbox.c \
    mcontainerctrl.c \
    miconflow.c \
    mlayout_manager.c \
    mlinevbox.c \
    museritem.c

libncs4pad_la_LIBADD = misc/libmisc.la physics-animation/libphyanim.la pieces/libpieces.la
