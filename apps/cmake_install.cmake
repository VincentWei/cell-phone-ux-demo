# Install script for directory: /home/weiym/devel/minigui/cellphoneuxdemo.dn/apps

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/sample-activity/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/sample-navigation/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/common/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/screen-lock/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/phone-call/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/music/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/photo/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/camera/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/calc/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/dial/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/address-book/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/sys-setting/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/sms/cmake_install.cmake")
  include("/home/weiym/devel/minigui/cellphoneuxdemo.dn/apps/callhistory/cmake_install.cmake")

endif()

