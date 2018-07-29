AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CPPFLAGS=-Iinclude/ -Incs4pad/include -Incs4pad/include/pieces -Incs4pad/include/physics-animation

SUBDIRS = etc include common engines controls ncs4pad desktop apps inner-res res
DIST_SUBDIRS = test tools

bin_PROGRAMS=mguxdemo activity-loader

APP_LIBS = \
    engines/libengines.la \
    apps/libapps.la \
    desktop/libdesktop.la \
    common/libcommon.la \
    controls/libcontrols.la \
    ncs4pad/src/libncs4pad.la \
    inner-res/libinner-res.la

mguxdemo_SOURCES = main.cc
mguxdemo_LDADD = $(APP_LIBS) @DEP_LIBS@

activity_loader_SOURCES = test/apps/activity-loader.cc
activity_loader_LDADD = $(APP_LIBS) @DEP_LIBS@

EXTRA_DIST = LICENSE README.md

install-data-local:
	$(INSTALL_DATA) MiniGUI.cfg $(prefix)/bin/MiniGUI.cfg
	$(INSTALL_DATA) mgncs.cfg $(prefix)/bin/mgncs.cfg
	$(INSTALL_DATA) mgncs4touch.cfg $(prefix)/bin/mgncs4touch.cfg
