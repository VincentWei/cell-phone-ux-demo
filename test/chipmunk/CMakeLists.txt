add_executable(test-utils
    ${PROJECT_SOURCE_DIR}/ncs4pad/src/physics-animation/chipmunk-utils.c
    ${PROJECT_SOURCE_DIR}/ncs4pad/src/physics-animation/mspeedmeter.c
    test-framework.c
    test-utils.c
    )
target_link_libraries(test-utils ${MINIGUI_LIBRARIES} chipmunk)

add_executable(test-simple
    test-simple.c
    )
target_link_libraries(test-simple ${MINIGUI_LIBRARIES} chipmunk)

add_executable(test-inertia
    ${PROJECT_SOURCE_DIR}/ncs4pad/src/physics-animation/mspeedmeter.c
    test-framework.c
    test-inertia.c
    )
target_link_libraries(test-inertia ${MINIGUI_LIBRARIES} chipmunk)
