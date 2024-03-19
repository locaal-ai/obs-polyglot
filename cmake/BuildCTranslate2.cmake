# build the CTranslate2 library from source https://github.com/OpenNMT/CTranslate2.git

include(ExternalProject)
include(FetchContent)

if(APPLE)

  FetchContent_Declare(
    ctranslate2_fetch
    URL https://github.com/occ-ai/obs-ai-ctranslate2-dep/releases/download/1.1.1/libctranslate2-macos-Release-1.1.1.tar.gz
    URL_HASH SHA256=da04d88ecc1ea105f8ee672e4eab33af96e50c999c5cc8170e105e110392182b)
  FetchContent_MakeAvailable(ctranslate2_fetch)

  add_library(ct2 INTERFACE)
  target_link_libraries(ct2 INTERFACE "-framework Accelerate" ${ctranslate2_fetch_SOURCE_DIR}/lib/libctranslate2.a
                                      ${ctranslate2_fetch_SOURCE_DIR}/lib/libcpu_features.a)
  set_target_properties(ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ctranslate2_fetch_SOURCE_DIR}/include)
  target_compile_options(ct2 INTERFACE -Wno-shorten-64-to-32)

elseif(WIN32)

  FetchContent_Declare(
    ctranslate2_fetch
    URL https://github.com/occ-ai/obs-ai-ctranslate2-dep/releases/download/1.1.1/libctranslate2-windows-4.1.1-Release.zip
    URL_HASH SHA256=aa87073e663e4dfbbc9b9360d83bed847212309bea433c3b1888a33a51cb0db0)
  FetchContent_MakeAvailable(ctranslate2_fetch)

  add_library(ct2 INTERFACE)
  target_link_libraries(ct2 INTERFACE ${ctranslate2_fetch_SOURCE_DIR}/lib/ctranslate2.lib)
  set_target_properties(ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ctranslate2_fetch_SOURCE_DIR}/include)
  target_compile_options(ct2 INTERFACE /wd4267 /wd4244 /wd4305 /wd4996 /wd4099)

  install(FILES ${ctranslate2_fetch_SOURCE_DIR}/bin/ctranslate2.dll ${ctranslate2_fetch_SOURCE_DIR}/bin/libopenblas.dll
          DESTINATION "obs-plugins/64bit")

else()
  set(CT2_VERSION "4.1.1")
  set(CT2_URL "https://github.com/OpenNMT/CTranslate2.git")
  set(CT2_OPENBLAS_CMAKE_ARGS -DWITH_OPENBLAS=OFF)

  set(CT2_CMAKE_PLATFORM_OPTIONS -DBUILD_SHARED_LIBS=OFF -DOPENMP_RUNTIME=NONE -DCMAKE_POSITION_INDEPENDENT_CODE=ON)

  ExternalProject_Add(
    ct2_build
    GIT_REPOSITORY ${CT2_URL}
    GIT_TAG v${CT2_VERSION}
    GIT_PROGRESS 1
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
    BUILD_BYPRODUCTS <INSTALL_DIR>/${CT2_LIB_INSTALL_LOCATION}
    CMAKE_ARGS -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
               -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DWITH_CUDA=OFF
               -DWITH_MKL=OFF
               -DWITH_TESTS=OFF
               -DWITH_EXAMPLES=OFF
               -DWITH_TFLITE=OFF
               -DWITH_TRT=OFF
               -DWITH_PYTHON=OFF
               -DWITH_SERVER=OFF
               -DWITH_COVERAGE=OFF
               -DWITH_PROFILING=OFF
               -DBUILD_CLI=OFF
               ${CT2_OPENBLAS_CMAKE_ARGS}
               ${CT2_CMAKE_PLATFORM_OPTIONS})
  ExternalProject_Get_Property(ct2_build INSTALL_DIR)

  add_library(ct2::ct2 STATIC IMPORTED GLOBAL)
  add_dependencies(ct2::ct2 ct2_build)
  set_target_properties(ct2::ct2 PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/${CT2_LIB_INSTALL_LOCATION})

  add_library(ct2 INTERFACE)
  target_link_libraries(ct2 INTERFACE ct2::ct2)
  set_target_properties(ct2::ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)

endif()
