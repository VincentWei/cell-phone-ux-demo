include_directories(${PROJECT_SOURCE_DIR}/apps/address-book)

add_executable(test_ContactsLoadHelper test-ContactsLoadHelper.cc 
    ../../common/ContentResolver.cc ../../common/ContactsProvider.cc
    ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
    ../../common/SQLiteCursor.cc ../../common/ContentValues.cc
    ../../common/common.cc ${PROJECT_SOURCE_DIR}/apps/address-book/ContactsLoadHelper.cc)
target_link_libraries(test_ContactsLoadHelper gtest ${SQLITE3_LIBRARIES})
