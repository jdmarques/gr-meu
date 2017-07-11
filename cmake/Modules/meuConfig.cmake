INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_MEU meu)

FIND_PATH(
    MEU_INCLUDE_DIRS
    NAMES meu/api.h
    HINTS $ENV{MEU_DIR}/include
        ${PC_MEU_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    MEU_LIBRARIES
    NAMES gnuradio-meu
    HINTS $ENV{MEU_DIR}/lib
        ${PC_MEU_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MEU DEFAULT_MSG MEU_LIBRARIES MEU_INCLUDE_DIRS)
MARK_AS_ADVANCED(MEU_LIBRARIES MEU_INCLUDE_DIRS)

