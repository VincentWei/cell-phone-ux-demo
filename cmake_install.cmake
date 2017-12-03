# Install script for directory: /home/weiym/devel/minigui/cellphoneuxdemo.dn

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/etc" TYPE FILE FILES "/home/weiym/devel/minigui/cellphoneuxdemo.dn/etc/system.db")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE DIRECTORY FILES "/home/weiym/devel/minigui/cellphoneuxdemo.dn/res" FILES_MATCHING REGEX "/[^/]*$" REGEX "/\\.svn$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE DIRECTORY FILES "/home/weiym/devel/minigui/cellphoneuxdemo.dn/etc" FILES_MATCHING REGEX "/[^/]*\\.xml$" REGEX "/[^/]*\\.skin$" REGEX "/\\.in$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES
    "/home/weiym/devel/minigui/cellphoneuxdemo.dn/MiniGUI.cfg"
    "/home/weiym/devel/minigui/cellphoneuxdemo.dn/mgncs4pad.cfg"
    "/home/weiym/devel/minigui/cellphoneuxdemo.dn/mgncs.cfg"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/weiym/devel/minigui/cellphoneuxdemo.dn/CMakeFiles/CMakeRelink.dir/cellphone")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "/home/weiym/devel/minigui/cellphoneuxdemo.dn/gvfb.exe"
    "/home/weiym/devel/minigui/cellphoneuxdemo.dn/run_cellphone.sh"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/ncs4pad/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/desktop/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/common/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/controls/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/inner-res/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/weiym/devel/minigui/cellphoneuxdemo.dn/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
