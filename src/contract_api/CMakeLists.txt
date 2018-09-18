
cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost COMPONENTS program_options filesystem system thread log REQUIRED)




set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

include_directories("${CMAKE_SOURCE_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../contract")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ethash/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../leveldb/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../jsoncpp/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../univalue/include")


file(GLOB sources "*.cpp")
file(GLOB headers "*.h")

add_library(contract_api ${sources} ${headers})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pipe -O2 -Wstack-protector -fstack-protector-all -fPIC -fvisibility=hidden")


#Add this line to pass -isystem flag and sysroot include directory to g++ compiler 
target_include_directories(contract_api SYSTEM PUBLIC ${BOOST_INCLUDEDIR})
