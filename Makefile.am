AUTOMAKE_OPTIONS = foreign

SUBDIRS = etc include common controls ncs4pad desktop apps inner-res res
DIST_SUBDIRS = test tools

bin_PROGRAMS=mguxdemo

mguxdemo_SOURCES = main.cc
mguxdemo_LDADD = common/libcommon.a \
                controls/libcontrols.a \
                ncs4pad/src/libncs4pad.a \
                desktop/libdesktop.a \
                apps/libapps.a \
                inner-res/libinnerres.a \
                $(DEP_LIBS)

EXTRA_DIST = LICENSE README.md

