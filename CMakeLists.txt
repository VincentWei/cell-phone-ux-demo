cmake_minimum_required(VERSION 2.6)
project (CellphoneUXDemo)

SET (CMAKE_BUILD_TYPE Debug)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall")
IF (WIN32)
   SET (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} /EHsc") # XXX: for boost
ENDIF (WIN32)

LIST (APPEND CMAKE_MODULE_PATH
    "${CMAKE_INSTALL_PREFIX}/share/minigui/cmake/Modules"
    )

include (common)
############## BASE ################
option(enable_ncs4pad_test "Enable testcase of ncs4pad" OFF)
option(enable_testcase "Enable Testcase" OFF)
option(enable_opengles "Enable opengles support" OFF)
option(debug_scrollview "Debug ScrollViewPiece" OFF)
mg_declare_option(with_arch STRING "x86" "with_arch: x86/s3c6410")
if (with_arch STREQUAL "s3c6410")
    mg_set_value (__ARCH_S3C6410__ ON)
else ()
    mg_set_value (__ARCH_X86__ ON)
endif (with_arch STREQUAL "s3c6410")

option(enable_app_test "Enable app test" OFF)
option(enable_common_test "Enable common Test" OFF)
option(enable_performance_test "Enable Performance Test" OFF)
option(enable_anim_fps_test "Enable Animation FPS Test" OFF)

option(enable_screen_calibration "Enable Screen Calibration" OFF)

set (enable_scrollview ON) # It's not an option any longer

if (debug_scrollview)
    ADD_DEFINITIONS(-DPHYANIM_DEBUG=1)
endif (debug_scrollview)

if (enable_screen_calibration)
    ADD_DEFINITIONS(-DSCREEN_CALIBRATION=1)
endif (enable_screen_calibration)


FIND_PACKAGE (FREETYPE)

INCLUDE (FindPkgConfig)
PKG_CHECK_MODULES (MINIGUI minigui)
IF (NOT MINIGUI_FOUND)
    MESSAGE(FATAL_ERROR "MiniGUI not found")
ENDIF (NOT MINIGUI_FOUND)
INCLUDE_DIRECTORIES (${MINIGUI_INCLUDE_DIRS})
LINK_DIRECTORIES (${MINIGUI_LIBRARY_DIRS})

PKG_CHECK_MODULES (MGUTILS mgutils)
INCLUDE_DIRECTORIES (${MGUTILS_INCLUDE_DIRS})
LINK_DIRECTORIES (${MGUTILS_LIBRARY_DIRS})

PKG_CHECK_MODULES (MGPLUS mgplus)
INCLUDE_DIRECTORIES (${MGPLUS_INCLUDE_DIRS})
LINK_DIRECTORIES (${MGPLUS_LIBRARY_DIRS})

PKG_CHECK_MODULES (MGNCS mgncs)
INCLUDE_DIRECTORIES (${MGNCS_INCLUDE_DIRS})
LINK_DIRECTORIES (${MGNCS_LIBRARY_DIRS})

PKG_CHECK_MODULES (MGEFF mgeff)
INCLUDE_DIRECTORIES (${MGEFF_INCLUDE_DIRS})
LINK_DIRECTORIES (${MGEFF_LIBRARY_DIRS})

PKG_CHECK_MODULES (MGNCS4PAD mgncs4pad)
INCLUDE_DIRECTORIES (${MGNCS4PAD_INCLUDE_DIRS})
LINK_DIRECTORIES (${MGNCS4PAD_LIBRARY_DIRS})

PKG_CHECK_MODULES (SQLITE3 sqlite3)
INCLUDE_DIRECTORIES (${SQLITE3_INCLUDE_DIRS})
LINK_DIRECTORIES (${SQLITE3_LIBRARY_DIRS})

# add opengles support
if (enable_opengles AND __ARCH_X86__)

    PKG_CHECK_MODULES (GLESv1_CM glesv1_cm)
    INCLUDE_DIRECTORIES (${GLESv1_CM_INCLUDE_DIRS})
    LINK_DIRECTORIES (${GLESv1_CM_LIBRARY_DIRS})

    PKG_CHECK_MODULES (EGL egl)
    INCLUDE_DIRECTORIES (${EGL_INCLUDE_DIRS})
    LINK_DIRECTORIES (${EGL_LIBRARY_DIRS})

    PKG_CHECK_MODULES (MGEGL mgegl)
    INCLUDE_DIRECTORIES (${MGEGL_INCLUDE_DIRS})
    LINK_DIRECTORIES (${MGEGL_LIBRARY_DIRS})

endif (enable_opengles AND __ARCH_X86__)

# Includes
include_directories ("${PROJECT_SOURCE_DIR}/" 
    "${PROJECT_SOURCE_DIR}" 
    "${PROJECT_SOURCE_DIR}/include" 
    "${PROJECT_SOURCE_DIR}/desktop")
include_directories(AFTER ${MINIGUI_INCLUDE_DIRS})
include_directories(AFTER "${MINIGUI_INCLUDE_DIRS}/mgncs/")
include_directories(AFTER "${MINIGUI_INCLUDE_DIRS}/mgncs/pieces/")
include_directories (
    ${PROJECT_SOURCE_DIR}/ncs4pad/include
    ${PROJECT_SOURCE_DIR}/ncs4pad/include/pieces
    ${PROJECT_SOURCE_DIR}/ncs4pad/include/physics-animation
    ${PROJECT_SOURCE_DIR}/ncs4pad/include/misc
    ${FREETYPE_INCLUDE_DIR}
    )
include_directories(${PROJECT_SOURCE_DIR}/inner-res)
include_directories(${PROJECT_SOURCE_DIR}/controls)

# Libraries
if (WIN32)

    find_library(XML2_LIBRARIES libxml2 xml2)
    if (NOT XML2_LIBRARIES)
        message(FATAL_ERROR "XML2 library file not found")
    endif(NOT XML2_LIBRARIES)

else () # not WIN32

    include(FindPkgConfig)

    pkg_check_modules (XML2   libxml-2.0>=2.6)
    if (XML2_FOUND)
        if(NOT "${XML2_INCLUDE_DIRS}" STREQUAL "")
            include_directories (${XML2_INCLUDE_DIRS})
        endif()
        link_directories (${XML2_LIBDIR})
    endif (XML2_FOUND)

endif (WIN32)

IF (enable_scrollview)
    ADD_DEFINITIONS(-DENABLE_SCROLLVIEW)
    LIST (APPEND _LIBS chipmunk)
ENDIF (enable_scrollview)

if (enable_opengles)
    if (__ARCH_X86__)
        list (APPEND _LIBS ${GLESv1_CM_LIBRARIES} ${EGL_LIBRARIES} ${MGEGL_LIBRARIES})
    elseif (__ARCH_S3C6410__)
        list (APPEND _LIBS -lgles11 -lEGL -lChunkAlloc -lmgegl)
    endif (__ARCH_X86__)
endif (enable_opengles)

IF (enable_anim_fps_test)
    ADD_DEFINITIONS(-DENABLE_ANIM_FPS_TEST)
ENDIF (enable_anim_fps_test)


LIST (APPEND _LIBS ${MINIGUI_LIBRARIES} ${XML2_LIBRARIES} ${FREETYPE_LIBRARY}
    ${MGPLUS_LIBRARIES} ${MGEFF_LIBRARIES} ${MGNCS_LIBRARIES} ${MGUTILS_LIBRARIES} ${MGNCS4PAD_LIBRARIES} ${SQLITE3_LIBRARIES})

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake_config.h.in"
    "${PROJECT_SOURCE_DIR}/config.h"
    )

# add sub directory
mg_add_sub_source_dirs (
    ncs4pad
    apps
    desktop
    common
    controls
    inner-res
    )

#message(STATUS "sources=${_source_list_}")

#add_executable(cellphone main.cc mgb_tswin.c ${_source_list_})
add_executable(cellphone main.cc ${_source_list_})
target_link_libraries(cellphone ${_LIBS})

add_executable(activity-loader ${PROJECT_SOURCE_DIR}/test/apps/activity-loader.cc ${_source_list_})
target_link_libraries(activity-loader ${_LIBS})

SET (DB_FILE ${CMAKE_BINARY_DIR}/etc/system.db)
CONFIGURE_FILE (etc/system.db.in ${DB_FILE} COPYONLY)
INSTALL(FILES ${DB_FILE} DESTINATION bin/etc)

IF (enable_testcase)
    add_subdirectory (test)
ENDIF (enable_testcase)

#INSTALL(FILES "${PROJECT_SOURCE_DIR}/res/font/Helvetica.ttf" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/minigui/res/font")

INSTALL(DIRECTORY res
    DESTINATION bin
    FILES_MATCHING
        PATTERN "*"
        PATTERN ".svn" EXCLUDE
    )

INSTALL(DIRECTORY etc
    DESTINATION bin
    FILES_MATCHING
        PATTERN "*.xml"
        PATTERN "*.skin"
        PATTERN ".in" EXCLUDE
    )

INSTALL(FILES MiniGUI.cfg mgncs4pad.cfg mgncs.cfg
    DESTINATION bin)

INSTALL(TARGETS cellphone
    DESTINATION bin)

IF (WIN32)
    INSTALL(PROGRAMS run_cellphone.sh
        DESTINATION bin)
ELSE(WIN32) # not WIN32
    INSTALL(PROGRAMS gvfb.exe run_cellphone.sh
        DESTINATION bin)
ENDIF (WIN32)

