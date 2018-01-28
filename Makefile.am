AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS=-Iinclude/ -Incs4pad/include -Incs4pad/include/pieces -Incs4pad/include/physics-animation

SUBDIRS = etc include common controls ncs4pad desktop apps inner-res res
DIST_SUBDIRS = test tools

bin_PROGRAMS=mguxdemo

mguxdemo_SOURCES = main.cc
mguxdemo_LDADD = common/libcommon.la \
                controls/libcontrols.la \
                ncs4pad/src/libncs4pad.la \
                desktop/libdesktop.la \
                apps/libapps.la \
                inner-res/libinner-res.la \
                @DEP_LIBS@

EXTRA_DIST = LICENSE README.md

install-data-local:
	$(INSTALL_DATA) MiniGUI.cfg $(prefix)/bin/MiniGUI.cfg
	$(INSTALL_DATA) mgncs.cfg $(prefix)/bin/mgncs.cfg
	$(INSTALL_DATA) mgncs4touch.cfg $(prefix)/bin/mgncs4touch.cfg
