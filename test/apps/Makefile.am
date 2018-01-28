AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS=-I../../

noinst_PROGRAMS=test_ContactsLoadHelper

test_ContactsLoadHelper_SOURCES = test-ContactsLoadHelper.cc \
    ../../common/ContentResolver.cc ../../common/ContactsProvider.cc\
    ../../common/ContentProvider.cc ../../common/SQLiteDB.cc\
    ../../common/SQLiteCursor.cc ../../common/ContentValues.cc\
    ../../common/common.cc ../../apps/address-book/ContactsLoadHelper.cc

test_ContactsLoadHelper_LDADD = ${SQLITE3_LIBRARIES}
