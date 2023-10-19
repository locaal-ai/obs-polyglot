
# build the CTranslate2 library from source https://github.com/OpenNMT/CTranslate2.git

include(ExternalProject)
include(FetchContent)

# Build with OpenBLAS
set(OpenBLAS_URL "https://github.com/xianyi/OpenBLAS/releases/download/v0.3.24/OpenBLAS-0.3.24-x64.zip")
set(OpenBLAS_SHA256 "6335128ee7117ea2dd2f5f96f76dafc17256c85992637189a2d5f6da0c608163")
FetchContent_Declare(
    openblas
    URL ${OpenBLAS_URL}
    URL_HASH SHA256=${OpenBLAS_SHA256})
FetchContent_MakeAvailable(openblas)
set(OpenBLAS_DIR ${openblas_SOURCE_DIR})
set(OPENBLAS_INCLUDE_DIR ${OpenBLAS_DIR}/include)

add_library(OpenBLAS STATIC IMPORTED)
set_target_properties(OpenBLAS PROPERTIES IMPORTED_LOCATION ${OpenBLAS_DIR}/lib/libopenblas.dll.a)
install(FILES ${OpenBLAS_DIR}/bin/libopenblas.dll DESTINATION "obs-plugins/64bit")

set(CT2_VERSION "3.20.0")
set(CT2_URL "https://github.com/OpenNMT/CTranslate2.git")

ExternalProject_Add(
    ct2_build
    DOWNLOAD_EXTRACT_TIMESTAMP true
    GIT_REPOSITORY ${CT2_URL}
    GIT_TAG v${CT2_VERSION}
    GIT_PROGRESS 1
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
  CMAKE_ARGS
    -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES_}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS=ON
    -DWITH_CUDA=OFF
    -DWITH_MKL=OFF
    -DWITH_OPENBLAS=ON
    -DOPENMP_RUNTIME=COMP
    -DWITH_TESTS=OFF
    -DWITH_EXAMPLES=OFF
    -DWITH_TFLITE=OFF
    -DWITH_TRT=OFF
    -DWITH_PYTHON=OFF
    -DWITH_SERVER=OFF
    -DWITH_COVERAGE=OFF
    -DWITH_PROFILING=OFF
    -DBUILD_CLI=OFF
    -DOPENBLAS_INCLUDE_DIR=${OPENBLAS_INCLUDE_DIR}
    -DOPENBLAS_LIBRARY=${OpenBLAS_DIR}/lib/libopenblas.dll.a)
ExternalProject_Get_Property(ct2_build INSTALL_DIR)

add_library(ct2::ct2 SHARED IMPORTED GLOBAL)
set_target_properties(
    ct2::ct2
    PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}ctranslate2${CMAKE_SHARED_LIBRARY_SUFFIX})
set_target_properties(
    ct2::ct2
    PROPERTIES IMPORTED_IMPLIB ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ctranslate2${CMAKE_STATIC_LIBRARY_SUFFIX})

install(FILES ${INSTALL_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}ctranslate2${CMAKE_SHARED_LIBRARY_SUFFIX}
    DESTINATION "obs-plugins/64bit")

add_library(ct2 INTERFACE)
add_dependencies(ct2 ct2_build)
target_link_libraries(ct2 INTERFACE ct2::ct2)
set_target_properties(ct2::ct2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)
