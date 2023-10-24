# build sentencepiece from "https://github.com/google/sentencepiece.git"

if(APPLE)

  include(FetchContent)

  FetchContent_Declare(
    sentencepiece_fetch
    URL https://github.com/obs-ai/obs-ai-ctranslate2-dep/releases/download/1.0.0/libsentencepiece-macos-Release-1.0.0.tar.gz
    URL_HASH SHA256=67f58a8e97c14db1bc69becd507ffe69326948f371bf874fe919157d7d65aff4)
  FetchContent_MakeAvailable(sentencepiece_fetch)
  add_library(sentencepiece INTERFACE)
  target_link_libraries(sentencepiece INTERFACE ${sentencepiece_fetch_SOURCE_DIR}/lib/libsentencepiece.a)
  set_target_properties(sentencepiece PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                                 ${sentencepiece_fetch_SOURCE_DIR}/include)

else()

  set(SP_URL
      "https://github.com/google/sentencepiece.git"
      CACHE STRING "URL of sentencepiece repository")

  if(UNIX)
    set(SP_CMAKE_PLATFORM_OPTIONS -DSPM_ENABLE_SHARED=OFF)
    if(APPLE)
      set(SP_CMAKE_PLATFORM_OPTIONS "${SP_CMAKE_PLATFORM_OPTIONS}" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13)
    endif()
    set(SENTENCEPIECE_INSTALL_LIB_LOCATION
        lib/${CMAKE_STATIC_LIBRARY_PREFIX}sentencepiece${CMAKE_STATIC_LIBRARY_SUFFIX})
  else()
    set(SP_CMAKE_PLATFORM_OPTIONS "")
    set(SENTENCEPIECE_INSTALL_LIB_LOCATION
        bin/${CMAKE_SHARED_LIBRARY_PREFIX}sentencepiece${CMAKE_SHARED_LIBRARY_SUFFIX})
  endif()

  include(ExternalProject)

  ExternalProject_Add(
    sentencepiece_build
    GIT_REPOSITORY ${SP_URL}
    GIT_TAG v0.1.99
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} -- -arch arm64
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
    BUILD_BYPRODUCTS <INSTALL_DIR>/${SENTENCEPIECE_INSTALL_LIB_LOCATION}
    CMAKE_ARGS -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${SP_CMAKE_PLATFORM_OPTIONS})
  ExternalProject_Get_Property(sentencepiece_build INSTALL_DIR)

  if(UNIX)
    add_library(libsentencepiece STATIC IMPORTED GLOBAL)
  else()
    add_library(libsentencepiece SHARED IMPORTED GLOBAL)
    set_target_properties(
      libsentencepiece
      PROPERTIES IMPORTED_IMPLIB
                 ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sentencepiece${CMAKE_STATIC_LIBRARY_SUFFIX})
  endif()
  add_dependencies(libsentencepiece sentencepiece_build)
  set_target_properties(libsentencepiece PROPERTIES IMPORTED_LOCATION
                                                    ${INSTALL_DIR}/${SENTENCEPIECE_INSTALL_LIB_LOCATION})

  add_library(sentencepiece INTERFACE)
  target_link_libraries(sentencepiece INTERFACE ${INSTALL_DIR}/${SENTENCEPIECE_INSTALL_LIB_LOCATION})
  set_target_properties(sentencepiece PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)

endif()
