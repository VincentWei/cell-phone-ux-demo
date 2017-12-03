IF (enable_app_test)
  add_subdirectory(apps)
ENDIF (enable_app_test)

IF (enable_common_test)
  add_subdirectory(common)
ENDIF (enable_common_test)

add_subdirectory(controls)

IF (enable_scrollview)
    add_subdirectory(chipmunk)
ENDIF (enable_scrollview)

IF (enable_performance_test)
    add_subdirectory(performance)
ENDIF (enable_performance_test)
