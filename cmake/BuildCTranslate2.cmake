# build the CTranslate2 library from source https://github.com/OpenNMT/CTranslate2.git

include(ExternalProject)
include(FetchContent)

if(APPLE)

  FetchContent_Declare(
    ctranslate2_fetch
    URL https://github.com/obs-ai/obs-ai-ctranslate2-dep/releases/download/1.0.0/libctranslate2-macos-Release-1.0.0.tar.gz
    URL_HASH SHA256=8e55a6ed4fb17ac556ad0e020ddab619584e3ceb4c9497a816f819bd8fd36443)
  FetchContent_MakeAvailable(ctranslate2_fetch)

  add_library(ct2 INTERFACE)
  target_link_libraries(ct2 INTERFACE "-framework Accelerate" ${ctranslate2_fetch_SOURCE_DIR}/lib/libctranslate2.a
                                      ${ctranslate2_fetch_SOURCE_DIR}/lib/libcpu_features.a)
  set_target_properties(ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ctranslate2_fetch_SOURCE_DIR}/include)
  target_compile_options(ct2 INTERFACE -Wno-shorten-64-to-32)

else()
  set(CT2_VERSION "3.20.0")
  set(CT2_URL "https://github.com/OpenNMT/CTranslate2.git")

  if(WIN32)
    # Build with OpenBLAS

    set(OpenBLAS_URL "https://github.com/xianyi/OpenBLAS/releases/download/v0.3.24/OpenBLAS-0.3.24-x64.zip")
    set(OpenBLAS_SHA256 "6335128ee7117ea2dd2f5f96f76dafc17256c85992637189a2d5f6da0c608163")
    FetchContent_Declare(
      openblas_fetch
      URL ${OpenBLAS_URL}
      URL_HASH SHA256=${OpenBLAS_SHA256})
    FetchContent_MakeAvailable(openblas_fetch)
    set(OpenBLAS_DIR ${openblas_fetch_SOURCE_DIR})
    set(OPENBLAS_INCLUDE_DIR ${OpenBLAS_DIR}/include)

    add_library(openblas STATIC IMPORTED)
    set_target_properties(openblas PROPERTIES IMPORTED_LOCATION ${OpenBLAS_DIR}/lib/libopenblas.dll.a)
    install(FILES ${OpenBLAS_DIR}/bin/libopenblas.dll DESTINATION "obs-plugins/64bit")

    set(CT2_OPENBLAS_CMAKE_ARGS -DOPENBLAS_INCLUDE_DIR=${OPENBLAS_INCLUDE_DIR}
                                -DOPENBLAS_LIBRARY=${OpenBLAS_DIR}/lib/libopenblas.dll.a -DWITH_OPENBLAS=ON)
  else()
    set(CT2_OPENBLAS_CMAKE_ARGS -DWITH_OPENBLAS=OFF)
  endif()

  if(UNIX)
    if(APPLE)
      set(CT2_CMAKE_PLATFORM_OPTIONS -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DBUILD_SHARED_LIBS=OFF -DWITH_ACCELERATE=ON
                                     -DOPENMP_RUNTIME=NONE -DCMAKE_OSX_ARCHITECTURES=arm64)
    else()
      set(CT2_CMAKE_PLATFORM_OPTIONS -DBUILD_SHARED_LIBS=OFF -DOPENMP_RUNTIME=NONE -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
    endif()
    set(CT2_LIB_INSTALL_LOCATION lib/${CMAKE_SHARED_LIBRARY_PREFIX}ctranslate2${CMAKE_STATIC_LIBRARY_SUFFIX})
  else()
    set(CT2_CMAKE_PLATFORM_OPTIONS -DBUILD_SHARED_LIBS=ON -DOPENMP_RUNTIME=COMP)
    set(CT2_LIB_INSTALL_LOCATION bin/${CMAKE_SHARED_LIBRARY_PREFIX}ctranslate2${CMAKE_SHARED_LIBRARY_SUFFIX})
  endif()

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

  if(UNIX)
    # Get cpu_features from the CTranslate2 build - only for x86_64 builds if(APPLE)
    # ExternalProject_Get_Property(ct2_build BINARY_DIR) add_library(ct2::cpu_features STATIC IMPORTED GLOBAL)
    # set_target_properties( ct2::cpu_features PROPERTIES IMPORTED_LOCATION
    # ${BINARY_DIR}/third_party/cpu_features/RelWithDebInfo/libcpu_features.a) endif()

    add_library(ct2::ct2 STATIC IMPORTED GLOBAL)
  else()
    add_library(ct2::ct2 SHARED IMPORTED GLOBAL)
    set_target_properties(
      ct2::ct2 PROPERTIES IMPORTED_IMPLIB
                          ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ctranslate2${CMAKE_STATIC_LIBRARY_SUFFIX})
    install(FILES ${INSTALL_DIR}/${CT2_LIB_INSTALL_LOCATION} DESTINATION "obs-plugins/64bit")
  endif()
  add_dependencies(ct2::ct2 ct2_build)
  set_target_properties(ct2::ct2 PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/${CT2_LIB_INSTALL_LOCATION})

  add_library(ct2 INTERFACE)
  if(APPLE)
    target_link_libraries(
      ct2
      INTERFACE
        "-framework Accelerate /Users/roy_shilkrot/Downloads/obs-ai-ctranslate2-dep/CTranslate2-3.20.0/release/universal/RelWithDebInfo/lib/libctranslate2.a"
    )
  else()
    target_link_libraries(ct2 INTERFACE ct2::ct2)
  endif()
  set_target_properties(ct2::ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)

endif()
