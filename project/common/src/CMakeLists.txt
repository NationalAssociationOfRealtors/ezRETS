include_directories (${ezrets_SOURCE_DIR}/project/common/include)

SET(COMMON_SRCS DataSource.cpp  EzRetsException.cpp)
add_library(ezrets-common STATIC ${COMMON_SRCS})
