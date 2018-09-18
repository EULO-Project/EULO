# Aleth: Ethereum C++ client, tools and libraries.
# Copyright 2018 Aleth Autors.
# Licensed under the GNU General Public License, Version 3. See the LICENSE file.

cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(ETH_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${ETH_CMAKE_DIR})

# Map current configuration to configurations of imported targets.
#fiber
enable_language(C)
find_package(Boost COMPONENTS program_options filesystem system thread log REQUIRED)

project(contract)
set(PROJECT_VERSION 1.4.0rc3)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils")

if(WIN32)
add_definitions(-DWIN32)
endif()

# Features:
option(VMTRACE "Enable VM tracing" OFF)
option(EVM_OPTIMIZE "Enable VM optimizations (can distort tracing)" ON)
option(FATDB "Enable fat state database" ON)
option(PARANOID "Enable additional checks when validating transactions (deprecated)" OFF)
option(MINIUPNPC "Build with UPnP support" OFF)
option(FASTCTEST "Enable fast ctest" OFF)

if (FATDB)
    add_definitions(-DETH_FATDB)
endif ()

if (VMTRACE)
    add_definitions(-DETH_VMTRACE)
endif ()

#Global include path for all libs.
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/utils")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../leveldb/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ethash/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../snappy")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../libscrypt")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../libff")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../libff/libff")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../jsoncpp/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../secp256k1/include")

option(ALETH_INTERPRETER_SHARED "Build aleth-interpreter as a shared library" OFF)

#add_subdirectory(libdevcore)
#add_subdirectory(libdevcrypto)
#add_subdirectory(libethcore)
#add_subdirectory(libevm)
#add_subdirectory(libethereum)
#add_subdirectory(libethashseal)

#add_library(contract STATIC $<TARGET_OBJECTS:devcore_obj>
#			    $<TARGET_OBJECTS:devcrypto_obj>
#			    $<TARGET_OBJECTS:ethcore_obj>
#			    $<TARGET_OBJECTS:evm_obj>
#			    $<TARGET_OBJECTS:ethereum_obj>
#			    $<TARGET_OBJECTS:ethashseal_obj>
#)

 

file(GLOB_RECURSE contract_sources "*.cpp" "*.h")
add_library(contract ${contract_sources})
target_include_directories(contract SYSTEM PUBLIC ${BOOST_INCLUDEDIR})
target_link_libraries(contract devcore devcrypto)



if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pipe -O2 -Wstack-protector -fstack-protector-all -fvisibility=hidden")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pipe -O2 -Wstack-protector -fstack-protector-all -fPIC -fvisibility=hidden")
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)


if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
include(CPack)
