
set(CppHTTPLib_URL "https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.14.1.tar.gz")
set(CppHTTPLib_HASH SHA256=2D4FB5544DA643E5D0A82585555D8B7502B4137EB321A4ABBB075E21D2F00E96)

# Download and unpack cpp-httplib at configure time
include(FetchContent)
FetchContent_Declare(
  cpphttplib
  URL ${CppHTTPLib_URL}
  URL_HASH ${CppHTTPLib_HASH})
FetchContent_MakeAvailable(cpphttplib)

add_library(cpphttplib INTERFACE)
target_include_directories(cpphttplib INTERFACE ${cpphttplib_SOURCE_DIR})
