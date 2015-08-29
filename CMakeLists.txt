project(lemon)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT4_INCLUDES} ${QT4_QTSQL_INCLUDE_DIR})

add_subdirectory( src )

add_subdirectory( icons )
add_subdirectory( styles )
add_subdirectory( images )
add_subdirectory( squeeze )
add_subdirectory( database_resources )

# MCH: add_subdirectory( doc )

#### CPACK STUFF ####
# Following:
# https://nowardev.wordpress.com/2009/12/27/how-to-create-a-debian-package-for-script-and-for-project-that-use-cmake/
SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 0)
 
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_PROVIDES "portobello")
SET(CPACK_DEBIAN_PACKAGE_NAME "portobello")
SET(CPACK_PACKAGE_DESCRIPTION "Social market management software.")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Portobello POS allows the easy management of a small supermarket which aims at equal redistribution of food and other material donations.")
SET(CPACK_PACKAGE_VENDOR "Portobello Modena")
SET(CPACK_PACKAGE_CONTACT "software@portobellomodena.it ")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
 
 SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgcc1, libqt4-dbus, libqt4-network, libqt4-sql-mysql, libqt4-xml, libqt4-svg, libqtcore4, libqtgui4, libstdc++6, libkdeui5, mysql-client, cheese")
 
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
SET(CPACK_DEBIAN_PACKAGE_SECTION "business")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
 
SET(CPACK_COMPONENTS_ALL Libraries ApplicationData)
INCLUDE(CPack)
 
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
