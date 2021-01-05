# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# Choose an appropriate compiler prefix

# for classical mingw32
# see http://www.mingw.org/
#set(COMPILER_PREFIX "i586-mingw32msvc")

# for 32 or 64 bits mingw-w64
# see http://mingw-w64.sourceforge.net/
if("$ENV{RABBIT_BUILD_CROSS_HOST}")
    set(COMPILER_PREFIX "$ENV{RABBIT_BUILD_CROSS_HOST}")
else("$ENV{RABBIT_BUILD_CROSS_HOST}")
    if("$ENV{BUILD_ARCH}" STREQUAL "x64")
        set(COMPILER_PREFIX "x86_64-w64-mingw32")
    else()
        set(COMPILER_PREFIX "i686-w64-mingw32")
    endif()
endif("$ENV{RABBIT_BUILD_CROSS_HOST}")

# which compilers to use for C and C++
find_program(CMAKE_RC_COMPILER NAMES ${COMPILER_PREFIX}-windres)
find_program(CMAKE_C_COMPILER NAMES ${COMPILER_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${COMPILER_PREFIX}-g++)
find_program(CMAKE_ASM_COMPILER NAMES ${COMPILER_PREFIX}-gcc)
find_program(CMAKE_AR NAMES ${COMPILER_PREFIX}-gcc-ar)
find_program(CMAKE_NM NAMES ${COMPILER_PREFIX}-gcc-nm)
find_program(CMAKE_RANLIB NAMES ${COMPILER_PREFIX}-gcc-ranlib)
#SET(CMAKE_RC_COMPILER ${COMPILER_PREFIX}-windres)
#SET(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
#SET(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)
#SET(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-gcc)
#SET(CMAKE_AR ${COMPILER_PREFIX}-gcc-ar)
#SET(CMAKE_NM ${COMPILER_PREFIX}-gcc-nm)
#SET(CMAKE_RANLIB ${COMPILER_PREFIX}-gcc-ranlib)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH $ENV{RABBIT_BUILD_CROSS_SYSROOT}) 
SET(CMAKE_SYSROOT $ENV{RABBIT_BUILD_CROSS_SYSROOT})
# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host and target environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
#include_directories(SYSTEM ${RABBIT_BUILD_CROSS_STL_INCLUDE})
#link_directories($RABBIT_BUILD_CROSS_STL_LIBS)
