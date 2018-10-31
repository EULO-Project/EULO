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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/evmjit/include")


option(ALETH_INTERPRETER_SHARED "Build aleth-interpreter as a shared library" OFF)


add_definitions(-DEULO_BUILD)


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

 set(   contractfile

        libdevcore/Assertions.h
   libdevcore/Base64.cpp
   libdevcore/Base64.h
   libdevcore/Common.cpp
   libdevcore/Common.h
   libdevcore/CommonData.cpp
   libdevcore/CommonData.h
   libdevcore/CommonIO.cpp
   libdevcore/CommonIO.h
   libdevcore/CommonJS.cpp
   libdevcore/CommonJS.h
   libdevcore/concurrent_queue.h
   libdevcore/db.h
   libdevcore/debugbreak.h
   libdevcore/Exceptions.h
   libdevcore/FileSystem.cpp
   libdevcore/FileSystem.h
   libdevcore/FixedHash.cpp
   libdevcore/FixedHash.h
   libdevcore/Guards.cpp
   libdevcore/Guards.h
   libdevcore/Hash.cpp
   libdevcore/Hash.h
   libdevcore/Log.cpp
   libdevcore/Log.h
   libdevcore/MemoryDB.cpp
   libdevcore/MemoryDB.h
   libdevcore/OverlayDB.cpp
   libdevcore/OverlayDB.h
   libdevcore/picosha2.h
   libdevcore/RangeMask.h
   libdevcore/RLP.cpp
   libdevcore/RLP.h
   libdevcore/SHA3.cpp
   libdevcore/SHA3.h
   libdevcore/Terminal.h
   libdevcore/TransientDirectory.cpp
   libdevcore/TransientDirectory.h
   libdevcore/TrieCommon.cpp
   libdevcore/TrieCommon.h
   libdevcore/TrieDB.cpp
   libdevcore/TrieDB.h
   libdevcore/TrieHash.cpp
   libdevcore/TrieHash.h
   libdevcore/UndefMacros.h
   libdevcore/vector_ref.h
   libdevcore/Worker.cpp
   libdevcore/Worker.h
   libdevcrypto/AES.cpp
   libdevcrypto/AES.h
   libdevcrypto/Common.cpp
   libdevcrypto/Common.h
   libdevcrypto/CryptoPP.cpp
   libdevcrypto/CryptoPP.h
   libdevcrypto/ECDHE.cpp
   libdevcrypto/ECDHE.h
   libdevcrypto/Exceptions.h
   libdevcrypto/SecretStore.cpp
   libdevcrypto/SecretStore.h
   libethash/compiler.h
   libethash/data_sizes.h
   libethash/endian.h
   libethash/ethash.h
   libethash/fnv.h
   libethash/internal.c
   libethash/internal.h
   libethash/io.c
   libethash/io.h
   libethash/io_posix.c
#   libethash/io_win32.c
   libethash/mmap.h
#   libethash/mmap_win32.c
   libethash/sha3.c
   libethash/sha3.h
   libethash/util.c
   libethash/util.h
#   libethash/util_win32.c
   libethashseal/genesis/eip150Test.cpp
   libethashseal/genesis/eip158Test.cpp
   libethashseal/genesis/frontierTest.cpp
   libethashseal/genesis/homesteadTest.cpp
   libethashseal/genesis/mainNetwork.cpp
   libethashseal/genesis/mainNetworkTest.cpp
   libethashseal/genesis/metropolisTest.cpp
   libethashseal/genesis/euloMainNetwork.cpp
   libethashseal/genesis/euloTestNetwork.cpp
   libethashseal/genesis/ropsten.cpp
   libethashseal/genesis/transitionnetTest.cpp
   libethashseal/Ethash.cpp
   libethashseal/Ethash.h
   libethashseal/EthashAux.cpp
   libethashseal/EthashAux.h
   libethashseal/EthashCPUMiner.cpp
   libethashseal/EthashCPUMiner.h
   libethashseal/EthashProofOfWork.cpp
   libethashseal/EthashProofOfWork.h
   libethashseal/GenesisInfo.cpp
   libethashseal/GenesisInfo.h
   libethcore/ABI.cpp
   libethcore/ABI.h
   libethcore/BasicAuthority.cpp
   libethcore/BasicAuthority.h
   libethcore/BlockHeader.cpp
   libethcore/BlockHeader.h
   libethcore/ChainOperationParams.cpp
   libethcore/ChainOperationParams.h
   libethcore/Common.cpp
   libethcore/Common.h
   libethcore/CommonJS.cpp
   libethcore/CommonJS.h
   libethcore/Exceptions.h
   libethcore/ICAP.cpp
   libethcore/ICAP.h
   libethcore/KeyManager.cpp
   libethcore/KeyManager.h
   libethcore/Precompiled.cpp
   libethcore/Precompiled.h
   libethcore/SealEngine.cpp
   libethcore/SealEngine.h
   libethcore/Transaction.cpp
   libethcore/Transaction.h
   libethereum/Account.cpp
   libethereum/Account.h
   libethereum/BasicGasPricer.cpp
   libethereum/BasicGasPricer.h
   libethereum/Block.cpp
   libethereum/Block.h
   libethereum/BlockChain.cpp
   libethereum/BlockChain.h
   libethereum/BlockDetails.cpp
   libethereum/BlockDetails.h
   libethereum/BlockQueue.cpp
   libethereum/BlockQueue.h
   libethereum/ChainParams.cpp
   libethereum/ChainParams.h
   libethereum/CodeSizeCache.h
   libethereum/CommonNet.cpp
   libethereum/CommonNet.h
   libethereum/Defaults.cpp
   libethereum/Defaults.h
   libethereum/Executive.cpp
   libethereum/Executive.h
   libethereum/ExtVM.cpp
   libethereum/ExtVM.h
   libethereum/GasPricer.cpp
   libethereum/GasPricer.h
   libethereum/GenericFarm.h
   libethereum/GenericMiner.cpp
   libethereum/GenericMiner.h
   libethereum/GenesisInfo.cpp
   libethereum/GenesisInfo.h
   libethereum/Interface.cpp
   libethereum/Interface.h
   libethereum/LogFilter.cpp
   libethereum/LogFilter.h
   libethereum/State.cpp
   libethereum/State.h
   libethereum/Transaction.cpp
   libethereum/Transaction.h
   libethereum/TransactionQueue.cpp
   libethereum/TransactionQueue.h
   libethereum/TransactionReceipt.cpp
   libethereum/TransactionReceipt.h
   libethereum/VerifiedBlock.h
   libevm/All.h
   libevm/ExtVMFace.cpp
   libevm/ExtVMFace.h
#   libevm/JitVM.cpp
#   libevm/JitVM.h
#   libevm/SmartVM.cpp
#   libevm/SmartVM.h
   libevm/VM.cpp
   libevm/VM.h
   libevm/VMCalls.cpp
   libevm/VMConfig.h
   libevm/VMFace.h
   libevm/VMFactory.cpp
   libevm/VMFactory.h
   libevm/VMOpt.cpp
   libevm/VMValidate.cpp
   libevmcore/EVMSchedule.h
   libevmcore/Exceptions.h
   libevmcore/Instruction.cpp
   libevmcore/Instruction.h
   utils/json_spirit/json_spirit.h
   utils/json_spirit/json_spirit_error_position.h
   utils/json_spirit/json_spirit_reader.cpp
   utils/json_spirit/json_spirit_reader.h
   utils/json_spirit/json_spirit_reader_template.h
   utils/json_spirit/json_spirit_stream_reader.h
   utils/json_spirit/json_spirit_utils.h
   utils/json_spirit/json_spirit_value.cpp
   utils/json_spirit/json_spirit_value.h
   utils/json_spirit/json_spirit_writer.cpp
   utils/json_spirit/json_spirit_writer.h
   utils/json_spirit/json_spirit_writer_template.h
   utils/json_spirit/JsonSpiritHeaders.h
   ../libscrypt/b64.c
   ../libscrypt/b64.h
   ../libscrypt/crypto-mcf.c
   ../libscrypt/crypto-scrypt-saltgen.c
   ../libscrypt/crypto_scrypt-check.c
   ../libscrypt/crypto_scrypt-hash.c
   ../libscrypt/crypto_scrypt-hexconvert.c
   ../libscrypt/crypto_scrypt-hexconvert.h
   ../libscrypt/crypto_scrypt-nosse.c
   ../libscrypt/libscrypt.h
   ../libscrypt/sha256.c
   ../libscrypt/sha256.h
   ../libscrypt/slowequals.c
   ../libscrypt/slowequals.h
   ../libscrypt/sysendian.h

)

#file(GLOB_RECURSE contract_sources "*.cpp" "*.h")

add_library(contract ${contractfile})
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
