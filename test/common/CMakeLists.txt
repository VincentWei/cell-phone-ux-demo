LIST ( APPEND ACTIVITY_SOURCES 
    ../../common/utility.c
    ../../common/Activity.cc
    )

LIST (APPEND APP_LIBS ${_LIBS})

# add_executable(test_statemachine test-statemachine.cc ../../common/StateMachine.cc)
# target_link_libraries(test_statemachine gtest ${MINIGUI_LIBRARIES})

#add_executable(test_activity test-activity.cc ${ACTIVITY_SOURCES})
#target_link_libraries(test_activity gtest pthread ${APP_LIBS})

add_executable(test_GraphicsView test-GraphicsView.cc ../../common/GraphicsView.cc)
target_link_libraries(test_GraphicsView gtest ${MINIGUI_LIBRARIES})

# add_executable(test_ActivityStack test-ActivityStack.cc 
#     ../../common/utility.c
#     ../../common/Activity.cc ../../common/ActivityStack.cc)
# target_link_libraries(test_ActivityStack gtest 
#     ${XML2_LIBRARIES} ${MINIGUI_LIBRARIES} ${MGEFF_LIBRARIES} ${MGPLUS_LIBRARIES})

# add_executable(test_SettingService test-SettingService.cc ../../common/SettingService.cc
#     ../../common/ContentResolver.cc ../../common/SettingsProvider.cc
#     ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
#     ../../common/SQLiteCursor.cc ../../common/ContentValues.cc)
# target_link_libraries(test_SettingService gtest ${SQLITE3_LIBRARIES})

# add_executable(test_SmsService test-SmsService.cc ../../common/SmsService.cc
#     ../../common/ContentResolver.cc ../../common/SmsProvider.cc
#     ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
#     ../../common/SQLiteCursor.cc ../../common/ContentValues.cc
#     ../../common/TimeService)
# target_link_libraries(test_SmsService gtest ${SQLITE3_LIBRARIES} ${MINIGUI_LIBRARIES})

add_executable(test_ContentResolver test-ContentResolver.cc 
    ../../common/ContentResolver.cc ../../common/SettingsProvider.cc
    ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
    ../../common/SQLiteCursor.cc ../../common/ContentValues.cc)
target_link_libraries(test_ContentResolver gtest ${SQLITE3_LIBRARIES})

add_executable(test_CallLogService test-CallLogService.cc ../../common/CallLogService.cc
    ../../common/ContentResolver.cc ../../common/CallLogProvider.cc
    ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
    ../../common/SQLiteCursor.cc ../../common/ContentValues.cc)
target_link_libraries(test_CallLogService gtest ${SQLITE3_LIBRARIES})

add_executable(test_MediaStoreService test-MediaStoreService.cc ../../common/MediaStoreService.cc
    ../../common/ContentResolver.cc ../../common/MediaStoreProvider.cc
    ../../common/ContentProvider.cc ../../common/SQLiteDB.cc
    ../../common/SQLiteCursor.cc ../../common/ContentValues.cc
    ../../common/common)
target_link_libraries(test_MediaStoreService gtest ${SQLITE3_LIBRARIES})
