include_directories("${PROJECT_SOURCE_DIR}/controls")

LIST (APPEND APP_LIBS
  ${MINIGUI_LIBRARIES} ${MGNCS_LIBRARY} ${MGPLUS_LIBRARY} ${MGEFF_LIBRARY} ${MGUTILS_LIBRARY}
  ${MGNCS4PAD_LIBRARIES}
  )

add_executable(test_animationedit 
  test-animationedit.c 
  ../../controls/animationedit.c 
  ../../ncs4pad/src/misc/mfillboxex.c
  ../../ncs4pad/src/pieces/mtransroundpiece.c)
target_link_libraries(test_animationedit ${APP_LIBS})

add_executable(datepicker
  datepicker.c 
  )
target_link_libraries(datepicker ${APP_LIBS})
