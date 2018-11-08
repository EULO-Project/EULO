DIST_SUBDIRS = secp256k1 univalue
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

if EMBEDDED_LEVELDB
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/include
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/helpers/memenv
LIBLEVELDB += $(builddir)/leveldb/libleveldb.a
LIBMEMENV  += $(builddir)/leveldb/libmemenv.a

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
$(LIBLEVELDB): $(LIBMEMENV)

$(LIBLEVELDB) $(LIBMEMENV):
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
          OPT="$(CXXFLAGS) $(CPPFLAGS)"
endif

BITCOIN_CONFIG_INCLUDES=-I$(builddir)/config
BITCOIN_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

BITCOIN_INCLUDES += -I$(srcdir)/secp256k1/include
BITCOIN_INCLUDES += -I$(srcdir)/lz4/lib
BITCOIN_INCLUDES += -I$(srcdir)/contract_api
BITCOIN_INCLUDES += -I$(srcdir)/contract
BITCOIN_INCLUDES += -I$(srcdir)/contract/utils

BITCOIN_INCLUDES += $(UNIVALUE_CFLAGS)

LIBBITCOIN_SERVER=libbitcoin_server.a
LIBBITCOIN_WALLET=libbitcoin_wallet.a
LIBBITCOIN_COMMON=libbitcoin_common.a
LIBBITCOIN_CLI=libbitcoin_cli.a
LIBBITCOIN_UTIL=libbitcoin_util.a
LIBBITCOIN_CRYPTO=crypto/libbitcoin_crypto.a
LIBBITCOIN_ZEROCOIN=libzerocoin/libbitcoin_zerocoin.a
LIBBITCOINQT=qt/libbitcoinqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la
LIBLZ4=lz4/lib/liblz4.a
LIBCRYPTOPP=cryptopp/libcryptopp.a
LIBETHASH=ethash/lib/ethash/libethash.a
LIBCONTRACT=contract/libcontract.a
LIBCONTRACT_API=contract_api/libcontract_api.a
LIBJSONCPP=jsoncpp/src/lib_json/libjsoncpp.a
LIBSCRYPT=libscrypt/libscrypt.a
LIBSNAPPY=snappy/libsnappy.a
LIBFF=libff/libff/libff.a

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

$(LIBLZ4): $(wildcard lz4/lib/*.[ch]) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F) CC="$(CC)" 

$(LIBCRYPTOPP): $(wildcard cryptopp/*.cpp cryptopp/*.h) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) -f GNUmakefile $(@F) CC="$(CC)" CXX="$(CXX)" AR="$(AR)"

$(LIBETHASH): $(wildcard ethash/lib/ethash/*.[ch] ethash/lib/ethash/*.[ch]pp) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D)

$(LIBCONTRACT): $(wildcard contract/libdevcore/*.h contract/libdevcore/*.cpp contract/libdevcrypto/*.h contract/libdevcrypto/*.cpp contract/libethashseal/*.h contract/libethashseal/*.cpp contract/libethcore/*.h contract/libethcore/*.cpp contract/libethereum/*.h contract/libethereum/*.cpp contract/libevm/*.h contract/libevm/*.cpp contract/utils/*.h contract/utils/*.cpp) 
	$(AM_V_at)$(MAKE)  $(AM_MAKEFLAGS)  -C contract/  

$(LIBCONTRACT_API):$(wildcard contract_api/*.h contract_api/*.cpp) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C contract_api/

$(LIBJSONCPP): $(wildcard jsoncpp/src/lib_json/*.cpp jsoncpp/src/lib_json/*.h) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C jsoncpp/

$(LIBSCRYPT): $(wildcard libscrypt/*.[ch]) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F) CC="$(CC)"
 
$(LIBSNAPPY): $(wildcard snappy/*.h snappy/*.cc) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) 

$(LIBFF): $(wildcard libff/libff/common/* libff/libff/common/*/* libff/libff/algebra/*/*/*) 
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES = \
  crypto/libbitcoin_crypto.a \
  libbitcoin_util.a \
  libbitcoin_common.a \
  libzerocoin/libbitcoin_zerocoin.a \
  libbitcoin_server.a \
  libbitcoin_cli.a
if ENABLE_WALLET
BITCOIN_INCLUDES += $(BDB_CPPFLAGS)
EXTRA_LIBRARIES += libbitcoin_wallet.a
endif

if ENABLE_ZMQ
EXTRA_LIBRARIES += libbitcoin_zmq.a
endif

if BUILD_BITCOIN_LIBS
lib_LTLIBRARIES = libbitcoinconsensus.la
LIBBITCOIN_CONSENSUS=libbitcoinconsensus.la
else
LIBBITCOIN_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_BITCOIND
  bin_PROGRAMS += eulod
endif

if BUILD_BITCOIN_UTILS
  bin_PROGRAMS += eulo-cli eulo-tx
endif

.PHONY: FORCE
# eulo core #
BITCOIN_CORE_H = \
  activemasternode.h \
  accumulators.h \
  accumulatormap.h \
  addrman.h \
  alert.h \
  allocators.h \
  amount.h \
  base58.h \
  bip38.h \
  bloom.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compat/sanity.h \
  compressor.h \
  primitives/block.h \
  primitives/transaction.h \
  primitives/zerocoin.h \
  core_io.h \
  crypter.h \
  denomination_functions.h \
  obfuscation.h \
  obfuscation-relay.h \
  db.h \
  hash.h \
  init.h \
  kernel.h \
  swifttx.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  lz4io.h \
  main.h \
  masternode.h \
  masternode-payments.h \
  masternode-budget.h \
  masternode-sync.h \
  masternodeman.h \
  masternodeconfig.h \
  merkleblock.h \
  miner.h \
  mruset.h \
  netbase.h \
  net.h \
  noui.h \
  pow.h \
  protocol.h \
  pubkey.h \
  random.h \
  reverse_iterate.h \
  rpcclient.h \
  rpcprotocol.h \
  rpcserver.h \
  script/interpreter.h \
  script/script.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/script_error.h \
  serialize.h \
  spork.h \
  sporkdb.h \
  streams.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  torcontrol.h \
  txdb.h \
  tmpblocksmempool.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  undo.h \
  util.h \
  utilstrencodings.h \
  utilmoneystr.h \
  utiltime.h \
  validationinterface.h \
  version.h \
  wallet.h \
  wallet_ismine.h \
  walletdb.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h \
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h \
  contract_api/contractbase.h \
  contract_api/contractcomponent.h \
  contract_api/contractconfig.h \
  contract_api/euloDGP.h \
  contract_api/eulostate.h \
  contract_api/eulotransaction.h \
  contract_api/storageresults.h \
  compat/sanity.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libbitcoin_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between eulod and eulo-qt
libbitcoin_server_a_CPPFLAGS = $(BITCOIN_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libbitcoin_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  lz4io.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  pow.cpp \
  rest.cpp \
  rpcblockchain.cpp \
  rpcmasternode.cpp \
  rpcmasternode-budget.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  script/sigcache.cpp \
  sporkdb.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  tmpblocksmempool.cpp \
  txmempool.cpp \
  validationinterface.cpp \
  contract_api/contractcomponent.cpp \
  contract_api/euloDGP.cpp \
  contract_api/eulostate.cpp \
  contract_api/storageresults.cpp \
  $(BITCOIN_CORE_H)

if ENABLE_ZMQ
LIBBITCOIN_ZMQ=libbitcoin_zmq.a

libbitcoin_zmq_a_CPPFLAGS = $(BITCOIN_INCLUDES) $(ZMQ_CFLAGS)
libbitcoin_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbitcoin_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif

# wallet: shared between eulod and eulo-qt, but only linked
# when wallet enabled

libbitcoin_wallet_a_CFLAGS = -fPIC
libbitcoin_wallet_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_wallet_a_SOURCES = \
  activemasternode.cpp \
  bip38.cpp \
  denomination_functions.cpp \
  obfuscation.cpp \
  obfuscation-relay.cpp \
  db.cpp \
  crypter.cpp \
  swifttx.cpp \
  masternode.cpp \
  masternode-budget.cpp \
  masternode-payments.cpp \
  masternode-sync.cpp \
  masternodeconfig.cpp \
  masternodeman.cpp \
  rpcdump.cpp \
  primitives/zerocoin.cpp \
  rpcwallet.cpp \
  kernel.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  $(BITCOIN_CORE_H)

# crypto primitives library
crypto_libbitcoin_crypto_a_CFLAGS = -fPIC
crypto_libbitcoin_crypto_a_CPPFLAGS = $(BITCOIN_CONFIG_INCLUDES) -maes
crypto_libbitcoin_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/scrypt.cpp \
  crypto/cryptonight.cpp \
  crypto/ripemd160.cpp \
  crypto/aes_helper.c \
  crypto/blake.c \
  crypto/bmw.c \
  crypto/groestl.c \
  crypto/jh.c \
  crypto/keccak.c \
  crypto/skein.c \
  crypto/common.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/scrypt.h \
  crypto/cryptonight.h \
  crypto/sha1.h \
  crypto/ripemd160.h \
  crypto/sph_blake.h \
  crypto/sph_bmw.h \
  crypto/sph_groestl.h \
  crypto/sph_jh.h \
  crypto/sph_keccak.h \
  crypto/sph_skein.h \
  crypto/sph_types.h \
  contract/utils/libscrypt/b64.c \
  contract/utils/libscrypt/b64.h \
  contract/utils/libscrypt/crypto-mcf.c \
  contract/utils/libscrypt/crypto_scrypt-check.c \
  contract/utils/libscrypt/crypto_scrypt-hash.c \
  contract/utils/libscrypt/crypto_scrypt-hexconvert.c \
  contract/utils/libscrypt/crypto_scrypt-hexconvert.h \
  contract/utils/libscrypt/crypto_scrypt-nosse.c \
  contract/utils/libscrypt/crypto-scrypt-saltgen.c \
  contract/utils/libscrypt/libscrypt.h \
  contract/utils/libscrypt/sha256.c \
  contract/utils/libscrypt/sha256.h \
  contract/utils/libscrypt/slowequals.c \
  contract/utils/libscrypt/slowequals.h \
  contract/utils/libscrypt/sysendian.h

# libzerocoin library
libzerocoin_libbitcoin_zerocoin_a_SOURCES = \
  libzerocoin/Accumulator.h \
  libzerocoin/AccumulatorProofOfKnowledge.h \
  libzerocoin/bignum.h \
  libzerocoin/Coin.h \
  libzerocoin/CoinSpend.h \
  libzerocoin/Commitment.h \
  libzerocoin/Denominations.h \
  libzerocoin/ParamGeneration.h \
  libzerocoin/Params.h \
  libzerocoin/SerialNumberSignatureOfKnowledge.h \
  libzerocoin/ZerocoinDefines.h \
  libzerocoin/Accumulator.cpp \
  libzerocoin/AccumulatorProofOfKnowledge.cpp \
  libzerocoin/Coin.cpp \
  libzerocoin/Denominations.cpp \
  libzerocoin/CoinSpend.cpp \
  libzerocoin/Commitment.cpp \
  libzerocoin/ParamGeneration.cpp \
  libzerocoin/Params.cpp \
  libzerocoin/SerialNumberSignatureOfKnowledge.cpp

# common: shared between eulod, and eulo-qt and non-server tools
libbitcoin_common_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_common_a_SOURCES = \
  accumulators.cpp \
  accumulatormap.cpp \
  allocators.cpp \
  amount.cpp \
  base58.cpp \
  bip38.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  primitives/zerocoin.cpp \
  core_read.cpp \
  core_write.cpp \
  hash.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  pubkey.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/sign.cpp \
  script/standard.cpp \
  script/script_error.cpp \
  spork.cpp \
  sporkdb.cpp \
  contract/libdevcore/Base64.cpp \
  contract/libdevcore/Base64.h \
  contract/libdevcore/Common.cpp \
  contract/libdevcore/Common.h \
  contract/libdevcore/CommonData.cpp \
  contract/libdevcore/CommonData.h \
  contract/libdevcore/CommonIO.cpp \
  contract/libdevcore/CommonIO.h \
  contract/libdevcore/CommonJS.cpp \
  contract/libdevcore/CommonJS.h \
  contract/libdevcore/FileSystem.cpp \
  contract/libdevcore/FileSystem.h \
  contract/libdevcore/FixedHash.cpp \
  contract/libdevcore/FixedHash.h \
  contract/libdevcore/Guards.cpp \
  contract/libdevcore/Guards.h \
  contract/libdevcore/Hash.cpp \
  contract/libdevcore/Hash.h \
  contract/libdevcore/Log.cpp \
  contract/libdevcore/Log.h \
  contract/libdevcore/MemoryDB.cpp \
  contract/libdevcore/MemoryDB.h \
  contract/libdevcore/OverlayDB.cpp \
  contract/libdevcore/OverlayDB.h \
  contract/libdevcore/RLP.cpp \
  contract/libdevcore/RLP.h \
  contract/libdevcore/SHA3.cpp \
  contract/libdevcore/SHA3.h \
  contract/libdevcore/TransientDirectory.cpp \
  contract/libdevcore/TransientDirectory.h \
  contract/libdevcore/TrieCommon.cpp \
  contract/libdevcore/TrieCommon.h \
  contract/libdevcore/Worker.cpp \
  contract/libdevcore/Worker.h \
  contract/libevm/ExtVMFace.cpp \
  contract/libevm/ExtVMFace.h \
  contract/libevm/VM.cpp \
  contract/libevm/VM.h \
  contract/libevm/VMOpt.cpp \
  contract/libevm/VMCalls.cpp \
  contract/libevm/VMFactory.cpp \
  contract/libevm/VMFactory.h \
  contract/libevmcore/Instruction.cpp \
  contract/libevmcore/Instruction.h \
  contract/libevmcore/Exceptions.h \
  contract/libevmcore/EVMSchedule.h \
  contract/libethereum/Account.cpp \
  contract/libethereum/Defaults.cpp \
  contract/libethereum/GasPricer.cpp \
  contract/libethereum/State.cpp \
  contract/libethcore/ABI.cpp \
  contract/libethcore/ChainOperationParams.cpp \
  contract/libethcore/Common.cpp \
  contract/libethcore/ICAP.cpp \
  contract/libethcore/Precompiled.cpp \
  contract/libdevcore/TrieDB.cpp \
  contract/libdevcrypto/Common.cpp \
  contract/libdevcrypto/CryptoPP.cpp \
  contract/libdevcrypto/AES.cpp \
  contract/libdevcrypto/ECDHE.cpp \
  contract/libethashseal/GenesisInfo.cpp \
  contract/libethereum/ChainParams.cpp \
  contract/libethcore/Transaction.cpp \
  contract/libethereum/Transaction.cpp \
  contract/libethereum/Executive.cpp \
  contract/libethereum/ExtVM.cpp \
  contract/libethereum/Block.cpp \
  contract/libethereum/BlockChain.cpp \
  contract/libethereum/BlockDetails.cpp \
  contract/libethereum/TransactionQueue.cpp \
  contract/libethereum/BlockQueue.cpp \
  contract/libethcore/BlockHeader.cpp \
  contract/libdevcore/RLP.cpp \
  contract/libethereum/TransactionReceipt.cpp \
  contract/libethcore/SealEngine.cpp \
  contract/libdevcore/TrieHash.cpp \
  contract/libethereum/GenesisInfo.cpp \
  contract/libethashseal/Ethash.cpp \
  contract/libethashseal/EthashCPUMiner.cpp \
  contract/libethashseal/EthashAux.cpp \
  contract/libethashseal/EthashProofOfWork.cpp \
  contract/libethereum/Account.h \
  contract/libethereum/Defaults.h \
  contract/libethereum/GasPricer.h \
  contract/libethereum/State.h \
  contract/libethcore/ABI.h \
  contract/libethcore/ChainOperationParams.h \
  contract/libethcore/Common.h \
  contract/libethcore/ICAP.h \
  contract/libethcore/Precompiled.h \
  contract/libdevcore/TrieDB.h \
  contract/libdevcrypto/Common.h \
  contract/libdevcrypto/CryptoPP.h \
  contract/libdevcrypto/AES.h \
  contract/libdevcrypto/ECDHE.h \
  contract/libethashseal/GenesisInfo.h \
  contract/libethereum/ChainParams.h \
  contract/libethcore/Transaction.h \
  contract/libethcore/Exceptions.h \
  contract/libethereum/Transaction.h \
  contract/libethereum/Executive.h \
  contract/libethereum/ExtVM.h \
  contract/libethereum/Block.h \
  contract/libethereum/BlockChain.h \
  contract/libethereum/BlockDetails.h \
  contract/libethereum/TransactionQueue.h \
  contract/libethereum/BlockQueue.h \
  contract/libethcore/BlockHeader.h \
  contract/libdevcore/RLP.h \
  contract/libethereum/TransactionReceipt.h \
  contract/libethcore/SealEngine.h \
  contract/libdevcore/TrieHash.h \
  contract/libethereum/GenesisInfo.h \
  contract/libethashseal/Ethash.h \
  contract/libethashseal/EthashCPUMiner.h \
  contract/libethashseal/EthashAux.h \
  contract/libethashseal/EthashProofOfWork.h \
  contract/libethash/internal.c \
  contract/libethash/io.c \
  contract/libethash/sha3.c \
  contract/libethash/internal.h \
  contract/libethash/io.h \
  contract/libethash/sha3.h \
  contract/libdevcore/vector_ref.h \
  contract/libdevcore/Exceptions.h \
  contract/libdevcore/db.h \
  contract/libdevcore/concurrent_queue.h \
  contract/libdevcore/Terminal.h \
  contract/libethereum/CodeSizeCache.h \
  contract/libethereum/VerifiedBlock.h \
  contract/libdevcore/Assertions.h \
  contract/libdevcore/debugbreak.h \
  contract/libethereum/GenericMiner.h \
  contract/libevm/VMFace.h \
  contract/libethereum/GenericFarm.h \
  contract/libethereum/Interface.h \
  contract/libethcore/CommonJS.h \
  contract/libdevcore/picosha2.h \
  contract/libevm/VMConfig.h \
  contract/libethereum/LogFilter.h \
  contract/libdevcrypto/Exceptions.h \
  contract/libethashseal/genesis/mainNetwork.cpp \
  contract/libethashseal/genesis/eip150Test.cpp \
  contract/libethashseal/genesis/eip158Test.cpp \
  contract/libethashseal/genesis/frontierTest.cpp \
  contract/libethashseal/genesis/homesteadTest.cpp \
  contract/libethashseal/genesis/mainNetworkTest.cpp \
  contract/libethashseal/genesis/metropolisTest.cpp \
  contract/libethashseal/genesis/euloMainNetwork.cpp \
  contract/libethashseal/genesis/euloTestNetwork.cpp \
  contract/libethashseal/genesis/ropsten.cpp \
  contract/libethashseal/genesis/transitionnetTest.cpp \
  contract/libethash/ethash.h \
  contract/libethash/compiler.h \
  contract/libethash/data_sizes.h \
  contract/libethash/fnv.h \
  contract/libethash/mmap.h \
  contract/libethash/endian.h \
  contract/libethash/io.h \
  contract/libethash/ethash.h \
  contract/libethash/internal.h \
  contract/libethash/mmap_win32.c \
  contract/utils/json_spirit/JsonSpiritHeaders.h \
  contract/utils/json_spirit/json_spirit.h \
  contract/utils/json_spirit/json_spirit_error_position.h \
  contract/utils/json_spirit/json_spirit_reader.h \
  contract/utils/json_spirit/json_spirit_reader_template.h \
  contract/utils/json_spirit/json_spirit_stream_reader.h \
  contract/utils/json_spirit/json_spirit_utils.h \
  contract/utils/json_spirit/json_spirit_value.h \
  contract/utils/json_spirit/json_spirit_writer.h \
  contract/utils/json_spirit/json_spirit_writer_template.h \
  $(BITCOIN_CORE_H)

if TARGET_WINDOWS
  libbitcoin_common_a_SOURCES += \
    contract/libethash/io_win32.c \
    contract/libethash/util_win32.c
else
  libbitcoin_common_a_SOURCES += contract/libethash/io_posix.c
endif

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libbitcoin_util_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_util_a_SOURCES = \
  allocators.cpp \
  compat/strnlen.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  random.cpp \
  rpcprotocol.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilstrencodings.cpp \
  utilmoneystr.cpp \
  utiltime.cpp \
  $(BITCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libbitcoin_util_a_SOURCES += compat/glibc_compat.cpp
libbitcoin_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

# cli: shared between eulo-cli and eulo-qt
libbitcoin_cli_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(BITCOIN_CORE_H)

nodist_libbitcoin_util_a_SOURCES = $(srcdir)/obj/build.h
#

# bitcoind binary #
eulod_LDADD = \
  $(LIBBITCOIN_SERVER) \
  $(LIBBITCOIN_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_ZEROCOIN) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1) \
  $(LIBLZ4) 

if ENABLE_ZMQ
eulod_LDADD += $(LIBBITCOIN_ZMQ) $(ZMQ_LIBS)
endif

if ENABLE_WALLET
eulod_LDADD += libbitcoin_wallet.a
endif
eulod_SOURCES = eulod.cpp
#

if TARGET_WINDOWS
eulod_SOURCES += eulod-res.rc
endif

eulod_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(LIBCRYPTOPP) -lgmp
eulod_CPPFLAGS = $(BITCOIN_INCLUDES)
eulod_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)


# eulo-cli binary #
eulo_cli_LDADD = \
  $(LIBBITCOIN_CLI) \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_UTIL) \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS) \
  $(EVENT_LIBS)

eulo_cli_SOURCES = \
  eulo-cli.cpp

eulo_cli_CPPFLAGS = $(BITCOIN_INCLUDES) $(EVENT_CFLAGS)
#

# eulo-tx binary #
eulo_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_ZEROCOIN) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBSECP256K1) \
  $(BOOST_LIBS) \
  $(CRYPTO_LIBS)

eulo_tx_SOURCES = eulo-tx.cpp
eulo_tx_CPPFLAGS = $(BITCOIN_INCLUDES)
#
eulo_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
eulo_cli_SOURCES += eulo-cli-res.rc
endif
eulo_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if BUILD_BITCOIN_LIBS
include_HEADERS = script/bitcoinconsensus.h
libbitcoinconsensus_la_SOURCES = \
  allocators.cpp \
  primitives/transaction.cpp \
  crypto/hmac_sha512.cpp \
  crypto/scrypt.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/ripemd160.cpp \
  hash.cpp \
  pubkey.cpp \
  script/script.cpp \
  script/interpreter.cpp \
  script/bitcoinconsensus.cpp \
  uint256.cpp \
  utilstrencodings.cpp

if GLIBC_BACK_COMPAT
  libbitcoinconsensus_la_SOURCES += compat/glibc_compat.cpp
  libbitcoinconsensus_la_SOURCES += compat/glibcxx_compat.cpp
endif

libbitcoinconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libbitcoinconsensus_la_LIBADD = $(LIBSECP256K1) $(BOOST_LIBS)
libbitcoinconsensus_la_CPPFLAGS = -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_BITCOIN_INTERNAL
if USE_LIBSECP256K1
libbitcoinconsensus_la_LIBADD += secp256k1/libsecp256k1.la
endif
endif

CLEANFILES = $(EXTRA_LIBRARIES) leveldb/libleveldb.a leveldb/libmemenv.a
CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C lz4 clean
	-$(MAKE) -C univalue clean
	-$(MAKE) -C cryptopp clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES)
	$(AM_V_GEN) $(WINDRES) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(CXXFLAGS)  -c -o $@ $<

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(abspath $(<D) $<)

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
