# build sentencepiece from "https://github.com/google/sentencepiece.git"

include(ExternalProject)

set(SP_URL
    "https://github.com/google/sentencepiece.git"
    CACHE STRING "URL of sentencepiece repository")

if(APPLE)
  set(SP_CMAKE_PLATFORM_OPTIONS "-DSPM_ENABLE_SHARED=OFF") # -DCMAKE_OSX_ARCHITECTURES=x86_64$<SEMICOLON>arm64)
else()
  set(SP_CMAKE_PLATFORM_OPTIONS "")
endif()

ExternalProject_Add(
  sentencepiece_build
  DOWNLOAD_EXTRACT_TIMESTAMP true
  GIT_REPOSITORY ${SP_URL}
  GIT_TAG v0.1.99
  BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
  CMAKE_ARGS -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM} -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
             -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${SP_CMAKE_PLATFORM_OPTIONS})
ExternalProject_Get_Property(sentencepiece_build INSTALL_DIR)

if(WIN32)
  set(SENTENCEPIECE_INSTALL_LIB_LOCATION
      ${INSTALL_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}sentencepiece${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
  set(SENTENCEPIECE_INSTALL_LIB_LOCATION
      ${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}sentencepiece${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

if(APPLE)
  add_library(libsentencepiece STATIC IMPORTED GLOBAL)
else()
  add_library(libsentencepiece SHARED IMPORTED GLOBAL)
  set_target_properties(
    libsentencepiece
    PROPERTIES IMPORTED_IMPLIB
               ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sentencepiece${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set_target_properties(libsentencepiece PROPERTIES IMPORTED_LOCATION ${SENTENCEPIECE_INSTALL_LIB_LOCATION})

add_library(sentencepiece INTERFACE)
add_dependencies(sentencepiece sentencepiece_build)
target_link_libraries(sentencepiece INTERFACE libsentencepiece)
set_target_properties(libsentencepiece PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)
