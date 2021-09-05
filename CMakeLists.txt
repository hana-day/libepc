cmake_minimum_required(VERSION 3.9)

project(libepc VERSION 1.0.0 LANGUAGES C CXX)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED OFF)
  set(CMAKE_C_EXTENSIONS OFF)
endif(NOT CMAKE_C_STANDARD)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif(NOT CMAKE_CXX_STANDARD)

option(LIBEPC_BUILD_TESTS "Build libepc's unit tests" OFF)

set(LIBEPC_PUBLIC_INCLUDE_DIR "include")

add_library(epc "")
target_sources(epc
  PRIVATE
  "epc/epc.cc"
  "epc/sgtin.cc"
  "epc/sscc.cc"
  "epc/sgln.cc"
  "epc/grai.cc"
  "epc/giai.cc"
  "epc/validation.h"
  "epc/validation.cc"
  "epc/encode.h"
  "epc/encode.cc"
  $<$<VERSION_GREATER:CMAKE_VERSION,3.2>:PUBLIC>
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/epc.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/sgtin.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/sscc.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/sgln.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/grai.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/giai.h"
  "${LIBEPC_PUBLIC_INCLUDE_DIR}/status.h"
  )

target_include_directories(epc
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  )

if(LIBEPC_BUILD_TESTS)
  enable_testing()
  
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(
    libepc_test
    "test/sgtin_test.cc"
    "test/sscc_test.cc"
    "test/sgln_test.cc"
    "test/grai_test.cc"
    "test/giai_test.cc"
    "test/encode_test.cc"
    )

  target_link_libraries(
    libepc_test
    epc
    gtest_main
    )

  target_include_directories(
    libepc_test
    PUBLIC
    "epc"
    )

  include(GoogleTest)
  gtest_discover_tests(libepc_test)
endif(LIBEPC_BUILD_TESTS)

