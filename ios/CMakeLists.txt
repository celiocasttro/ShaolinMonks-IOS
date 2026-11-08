cmake_minimum_required(VERSION 3.24)
project(ShaolinMonksIOSHost LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
  message(FATAL_ERROR "This host must be configured with -DCMAKE_SYSTEM_NAME=iOS")
endif()
if(NOT DEFINED SDL_SOURCE_DIR OR NOT EXISTS "${SDL_SOURCE_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "Pass -DSDL_SOURCE_DIR=/path/to/SDL3")
endif()

# Optional private original XBE used to build the recompiled game. Never commit it.
set(IOS_XBE_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/../assets/default.xbe" CACHE FILEPATH
    "Private original default.xbe used to identify the exact game release")
set(IOS_EXPECTED_XBE_SHA1 "")
if(EXISTS "${IOS_XBE_IMAGE}")
  file(SHA1 "${IOS_XBE_IMAGE}" IOS_EXPECTED_XBE_SHA1)
  message(STATUS "iOS host will verify XBE release SHA-1: ${IOS_EXPECTED_XBE_SHA1}")
else()
  message(STATUS "No IOS_XBE_IMAGE supplied; XBE release verification will be unavailable")
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${SDL_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/SDL3" EXCLUDE_FROM_ALL)

add_executable(ShaolinMonksIOS MACOSX_BUNDLE
  src/main.cpp
  src/game_data.mm
)
target_link_libraries(ShaolinMonksIOS PRIVATE SDL3-static "-framework Foundation")
target_include_directories(ShaolinMonksIOS PRIVATE "${SDL_SOURCE_DIR}/include")
target_compile_definitions(ShaolinMonksIOS PRIVATE IOS_EXPECTED_XBE_SHA1="${IOS_EXPECTED_XBE_SHA1}")

set_target_properties(ShaolinMonksIOS PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
  MACOSX_BUNDLE_GUI_IDENTIFIER "com.celiocasttro.shaolinmonks"
  MACOSX_BUNDLE_BUNDLE_NAME "Shaolin Monks"
  MACOSX_BUNDLE_BUNDLE_VERSION "1"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
  XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.celiocasttro.shaolinmonks"
  XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
  XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0"
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
)
