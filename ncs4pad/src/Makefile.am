AM_CPPFLAGS = -I../../include -I../include -I../include/misc -I../include/physics-animation -I../include/pieces

SUBDIRS = misc physics-animation pieces

noinst_LIBRARIES = libncs4pad.a

libncs4pad_a_SOURCES = \
    mcenterhbox.c \
    mcontainerctrl.c \
    miconflow.c \
    mlayout_manager.c \
    mlinevbox.c \
    museritem.c

libncs4pad_a_LIBADD = misc/libmisc.a physics-animation/libphyanim.a pieces/libpieces.a
