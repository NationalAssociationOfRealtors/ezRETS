include_directories (${ezrets_SOURCE_DIR}/project/common/include)

FILE(GLOB COMMON_SRCS *.cpp)
add_library(ezrets-common STATIC ${COMMON_SRCS})
