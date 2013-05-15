# Install script for directory: /home/cloto/emporium/styles/elegant

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/lemon/styles/elegant/backg.png;/lemon/styles/elegant/button.png;/lemon/styles/elegant/button_hover.png;/lemon/styles/elegant/checkbox_off.png;/lemon/styles/elegant/checkbox_on.png;/lemon/styles/elegant/checks_bg.png;/lemon/styles/elegant/editbox.png;/lemon/styles/elegant/groupbox.png;/lemon/styles/elegant/headers.png;/lemon/styles/elegant/headerview.png;/lemon/styles/elegant/headerview_hover.png;/lemon/styles/elegant/hgroupbox.png;/lemon/styles/elegant/loginBackground.png;/lemon/styles/elegant/loginBackground_1280x800.png;/lemon/styles/elegant/loginBackground_1280x1024.png;/lemon/styles/elegant/loginBackground_1024x768.png;/lemon/styles/elegant/passwordBackground.png;/lemon/styles/elegant/passwordBackground_wide.png;/lemon/styles/elegant/radiobutton_off.png;/lemon/styles/elegant/radiobutton_on.png;/lemon/styles/elegant/tira.png;/lemon/styles/elegant/dialog.png;/lemon/styles/elegant/elegant.qss;/lemon/styles/elegant/Schermata.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/lemon/styles/elegant" TYPE FILE FILES
    "/home/cloto/emporium/styles/elegant/backg.png"
    "/home/cloto/emporium/styles/elegant/button.png"
    "/home/cloto/emporium/styles/elegant/button_hover.png"
    "/home/cloto/emporium/styles/elegant/checkbox_off.png"
    "/home/cloto/emporium/styles/elegant/checkbox_on.png"
    "/home/cloto/emporium/styles/elegant/checks_bg.png"
    "/home/cloto/emporium/styles/elegant/editbox.png"
    "/home/cloto/emporium/styles/elegant/groupbox.png"
    "/home/cloto/emporium/styles/elegant/headers.png"
    "/home/cloto/emporium/styles/elegant/headerview.png"
    "/home/cloto/emporium/styles/elegant/headerview_hover.png"
    "/home/cloto/emporium/styles/elegant/hgroupbox.png"
    "/home/cloto/emporium/styles/elegant/loginBackground.png"
    "/home/cloto/emporium/styles/elegant/loginBackground_1280x800.png"
    "/home/cloto/emporium/styles/elegant/loginBackground_1280x1024.png"
    "/home/cloto/emporium/styles/elegant/loginBackground_1024x768.png"
    "/home/cloto/emporium/styles/elegant/passwordBackground.png"
    "/home/cloto/emporium/styles/elegant/passwordBackground_wide.png"
    "/home/cloto/emporium/styles/elegant/radiobutton_off.png"
    "/home/cloto/emporium/styles/elegant/radiobutton_on.png"
    "/home/cloto/emporium/styles/elegant/tira.png"
    "/home/cloto/emporium/styles/elegant/dialog.png"
    "/home/cloto/emporium/styles/elegant/elegant.qss"
    "/home/cloto/emporium/styles/elegant/Schermata.png"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

