project(ezrets)

# This is on until we learn more about cmake
SET(CMAKE_START_TEMP_FILE "" CACHE STRING "Foo")
SET(CMAKE_END_TEMP_FILE "" CACHE STRING "FOO")
SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Foo again")

SET(PROJECT_BINARY_DIR build)
ADD_DEFINITIONS(-DEZRETS_VERSION=\\\"2.0.7\\\")

FIND_PROGRAM(LIBRETS_CONFIG librets-config DOC "librets-config program")
IF (LIBRETS_CONFIG)
  EXEC_PROGRAM(${LIBRETS_CONFIG} ARGS "--cflags" OUTPUT_VARIABLE LIBRETS_CFLAGS)
  EXEC_PROGRAM(${LIBRETS_CONFIG} ARGS "--libs" OUTPUT_VARIABLE LIBRETS_LDFLAGS)
ELSE (LIBRETS_CONFIG)
  SET(LIBRETS_INCLUDE_DIRECTORY "" CACHE PATH "LIBRETS include directory")
ENDIF (LIBRETS_CONFIG)

#SET (LIBRETS_CONFIG ON CACHE FILEPATH "Where is your librets-config")

INCLUDE(CheckIncludeFile)
#FIND_PACKAGE(wxWidgets)
INCLUDE(FindwxWidgets)

IF (WIN32)
  INCLUDE(FindRuby)
ENDIF (WIN32)

FIND_PROGRAM(WITH_IODBC iodbc-config DOC "Do we have IODBC?")
if (WITH_IODBC)
  ADD_DEFINITIONS(-DHAVE_IODBC)
else (WITH_IODBC)
  CHECK_INCLUDE_FILE(sql.h SQL_H_FOUND)
  CHECK_INCLUDE_FILE(odbcinst.h ODBCINST_H_FOUND)
ENDIF (WITH_IODBC)

# This adds boost support
IF ( WIN32 )
  # Assumes BOOST_ROOT and BOOST_THREAD_* are set in your path statement.
  # If not then either add them to your path statement or edit the following
  # lines putting in the absolute paths to the boost includes and libraries.
  SET ( Boost_Root
#    $(BOOST_ROOT)
#    $ENV{BOOST_ROOT}
    "C:/boost/boost_1_33_0"
    CACHE PATH
    "Path to the boost root directory."
  )
  SET(BOOST_INCLUDE_PATH
    ${Boost_Root}
    CACHE PATH
    "Path to the Boost include files.")

  #SET(BOOST_REGEX_MT_LIB_PATH
  # ${Boost_Root}/bin/boost/libs/regex/build/libboost_regex.lib/vc-7_1/release/threading-multi
  #  CACHE PATH
  #  "Path to the Boost multi-threaded regular expression release libraries.")
  #SET(BOOST_REGEX_MT_D_LIB_PATH
  # ${Boost_Root}/bin/boost/libs/regex/build/libboost_regex.lib/vc-7_1/debug/threading-multi
  #  CACHE PATH
  #  "Path to the Boost multi-threaded regular expression debug libraries.")
  #SET(BOOST_THREAD_MT_LIB_PATH
  #  "$(BOOST_THREAD_MT)"
  #  CACHE PATH
  #  "Path to the Boost multi-threaded thread release libraries.")
  #SET(BOOST_THREAD_MT_D_LIB_PATH
  #  "$(BOOST_THREAD_MT_D)"
  #  CACHE PATH
  #  "Path to the Boost multi-threaded thread debug libraries.")
#  SET(BOOST_FS_MT_LIB_PATH ${Boost_Root}/bin/boost/libs/filesystem/build/libboost_filesystem.lib/vc-7_1/release/threading-multi CACHE PATH "Path to the Boost multi-threaded thread filesystem release libraries.")
#  SET(BOOST_FS_MT_D_LIB_PATH ${Boost_Root}/bin/boost/libs/filesystem/build/libboost_filesystem.lib/vc-7_1/debug/threading-multi CACHE PATH "Path to the Boost multi-threaded thread filesystem debug libraries.")
ELSE ( WIN32 )
  # In Linux the includes are generally in the system paths. I.e /usr/include/boost
  # In Linux the libs are generally in the system paths. I.e. /usr/lib ->
  # If you build and install boost yourself, the headers are in: /usr/local/include/boost-1_33/
  # and the libraries are in: /usr/local/lib/

  #  "Path to the Boost multi-threaded thread debug libraries.")
  SET(BOOST_INCLUDE_PATH /usr/local/include/boost-1_33/ CACHE PATH "Path to the Boost include files.")
ENDIF ( WIN32 )

ADD_SUBDIRECTORY(project/common/src build/ezrets-common)
