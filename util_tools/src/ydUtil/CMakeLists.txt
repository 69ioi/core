ADD_DEFINITIONS(-DYDUTIL_LIBRARY)
SET(LIB_NAME ydUtil)
SET(HEAD_PATH ${YD_SOURCE_DIR}/include/${LIB_NAME})
SET(SOURCE_PATH ${YD_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
  ${HEAD_PATH}/export.h
  ${HEAD_PATH}/Param.h
  ${HEAD_PATH}/CurlFile.h
  ${HEAD_PATH}/DownloadTask.h
  ${HEAD_PATH}/console.h
  ${HEAD_PATH}/common.h
  ${HEAD_PATH}/DataReadWriter.h
  ${HEAD_PATH}/matrix_math.h
  ${HEAD_PATH}/log.h
  ${HEAD_PATH}/StringUtils.h
  ${HEAD_PATH}/FindGeometryVisitor.h
  ${HEAD_PATH}/ImageChange.h
  ${HEAD_PATH}/colorchange.h
  ${HEAD_PATH}/geometry.h
  ${YDS_CONFIG_HEADER}
  )

SET(LIB_SOURCES
  ${SOURCE_PATH}/CMakeLists.txt
  ${SOURCE_PATH}/Param.cpp
  ${SOURCE_PATH}/CurlFile.cpp
  ${SOURCE_PATH}/DownloadTask.cpp
  ${SOURCE_PATH}/console.cpp
  ${SOURCE_PATH}/common.cpp
  ${SOURCE_PATH}/DataReadWriter.cpp
  ${SOURCE_PATH}/matrix_math.cpp
  ${SOURCE_PATH}/log.cpp
  ${SOURCE_PATH}/StringUtils.cpp
  ${SOURCE_PATH}/FindGeometryVisitor.cpp
  ${SOURCE_PATH}/ImageChange.cpp
  ${SOURCE_PATH}/colorchange.cpp
  ${SOURCE_PATH}/geometry.cpp
  )

ADD_DEFINITIONS(-DASIO_STANDALONE -DUSE_STANDALONE_ASIO -DNANOVG_GLEW -DNANOVG_GL2_IMPLEMENTATION)
INCLUDE_DIRECTORIES(
  ${OSG_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${YD_SOURCE_PATH}/include
  ${TINYXML2_INCLUDE_DIR}
  ${DEPEND_PATH}/include
  )

LINK_DIRECTORIES(${OSG_LIB_DIR} ${CMAKE_CURRENT_SOURCE_DIR}  ${DEPEND_PATH}/lib)
#########################################


SET(LIB_DEPS
  OPENTHREADS_LIBRARY
  OSGTEXT_LIBRARY
  OSG_LIBRARY
  OSGDB_LIBRARY
  CURL_LIBRARY
  OpenCV_LIBS
  )


PROJECT_ADD_LIBRARY(${LIB_NAME} SHARED)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CURL_INCLUDE_DIR}
  )

#########################################

TARGET_COMPILE_OPTIONS(${LIB_NAME} PUBLIC -D_SCL_SECURE_NO_WARNINGS)
IF(APPLE)
  TARGET_LINK_LIBRARIES(${LIB_NAME} objc)
ENDIF(APPLE)
