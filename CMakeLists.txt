project(ezrets)

INCLUDE(CheckIncludeFile)
FIND_PACKAGE(wxWidgets)

IF (WIN32)
   INCLUDE(FindRuby)
ENDIF (WIN32)

FIND_FILE(WITH_IODBC iodbc-config)
if (WITH_IODBC)
   ADD_DEFINITIONS(-DHAVE_IODBC)
else (WITH_IODBC)
	CHECK_INCLUDE_FILE(sql.h SQL_H_FOUND)
	CHECK_INCLUDE_FILE(odbcinst.h ODBCINST_H_FOUND)
ENDIF (WITH_IODBC)

add_subdirectory(project/common/src)
