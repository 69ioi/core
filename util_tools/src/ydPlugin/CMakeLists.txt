ADD_DEFINITIONS(-DYDPLUGIN_LIBRARY)


SET(LIB_NAME ydPlugin)

SET(HEADER_PATH ${YD_SOURCE_DIR}/include/${LIB_NAME})
SET(SOURCE_PATH ${YD_SOURCE_DIR}/src/${LIB_NAME})


SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/Plugin.h
  ${HEADER_PATH}/PluginManager.h
  ${HEADER_PATH}/PluginModule.h
)

SET( LIB_SOURCES
  ${SOURCE_PATH}/CMakeLists.txt
  ${SOURCE_PATH}/Plugin.cpp
  ${SOURCE_PATH}/PluginModule.cpp
  ${SOURCE_PATH}/PluginManager.cpp
)

IF(WIN32)
SET(LIB_SOURCES
    ${LIB_SOURCES}
    ${SOURCE_PATH}/win32/win32Plugin.cpp
    ${SOURCE_PATH}/win32/win32Plugin.h
    )
ELSE(WIN32)
    IF(APPLE)
        SET(LIB_SOURCES
            ${LIB_SOURCES}
            ${SOURCE_PATH}/mac/macPlugin.cpp
            ${SOURCE_PATH}/mac/macPlugin.h
            )
    ELSE(APPLE)
        SET(LIB_SOURCES
            ${LIB_SOURCES}
            ${SOURCE_PATH}/linux/linuxPlugin.cpp
            ${SOURCE_PATH}/linux/linuxPlugin.h
            )
    ENDIF(APPLE)

ENDIF(WIN32)

LINK_DIRECTORIES(${OSG_LIB_DIR} ${CMAKE_CURRENT_SOURCE_DIR}  ${DEPEND_PATH}/lib)

SET(LIB_INTERNALS
    ydUtil
	tinyxml2
  )

SET(LIB_DEPS
  TINYXML2_LIBRARY
  )

INCLUDE_DIRECTORIES(
  ${TINYXML2_INCLUDE_DIR}
  )

PROJECT_ADD_LIBRARY(${LIB_NAME})
