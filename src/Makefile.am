DIST_SUBDIRS = secp256k1
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)


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

DYNAMICCOIN_CONFIG_INCLUDES=-I$(builddir)/config
DYNAMICCOIN_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

DYNAMICCOIN_INCLUDES += -I$(srcdir)/secp256k1/include

LIBDYNAMICCOIN_SERVER=libdynamiccoin_server.a
LIBDYNAMICCOIN_WALLET=libdynamiccoin_wallet.a
LIBDYNAMICCOIN_COMMON=libdynamiccoin_common.a
LIBDYNAMICCOIN_CLI=libdynamiccoin_cli.a
LIBDYNAMICCOIN_UTIL=libdynamiccoin_util.a
LIBDYNAMICCOIN_CRYPTO=crypto/libdynamiccoin_crypto.a
LIBDYNAMICCOIN_UNIVALUE=univalue/libdynamiccoin_univalue.a
LIBDYNAMICCOINQT=qt/libdynamiccoinqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES = \
  crypto/libdynamiccoin_crypto.a \
  libdynamiccoin_util.a \
  libdynamiccoin_common.a \
  univalue/libdynamiccoin_univalue.a \
  libdynamiccoin_server.a \
  libdynamiccoin_cli.a
if ENABLE_WALLET
DYNAMICCOIN_INCLUDES += $(BDB_CPPFLAGS)
EXTRA_LIBRARIES += libdynamiccoin_wallet.a
endif

if BUILD_DYNAMICCOIN_LIBS
lib_LTLIBRARIES = libbitcoinconsensus.la
LIBDYNAMICCOIN_CONSENSUS=libbitcoinconsensus.la
else
LIBDYNAMICCOIN_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_DYNAMICCOIND
  bin_PROGRAMS += dynamiccoind
endif

if BUILD_DYNAMICCOIN_UTILS
  bin_PROGRAMS += dynamiccoin-cli dynamiccoin-tx
endif

.PHONY: FORCE
# dynamiccoin core #
DYNAMICCOIN_CORE_H = \
  addrman.h \
  alert.h \
  allocators.h \
  amount.h \
  base58.h \
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
  compressor.h \
  primitives/block.h \
  primitives/transaction.h \
  core_io.h \
  crypter.h \
  db.h \
  eccryptoverify.h \
  ecwrapper.h \
  GrsApi.h \
  hash.h \
  init.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
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
  streams.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  undo.h \
  util.h \
  utilstrencodings.h \
  utilmoneystr.h \
  utiltime.h \
  version.h \
  wallet.h \
  wallet_ismine.h \
  walletdb.h \
  compat/sanity.h

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libdynamiccoin_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between dynamiccoind and dynamiccoin-qt
libdynamiccoin_server_a_CPPFLAGS = $(DYNAMICCOIN_INCLUDES) $(MINIUPNPC_CPPFLAGS)
libdynamiccoin_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
  GrsApi.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  pow.cpp \
  rest.cpp \
  rpcblockchain.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  script/sigcache.cpp \
  timedata.cpp \
  txdb.cpp \
  txmempool.cpp \
  $(JSON_H) \
  $(DYNAMICCOIN_CORE_H)

# wallet: shared between dynamiccoind and dynamiccoin-qt, but only linked
# when wallet enabled
libdynamiccoin_wallet_a_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
libdynamiccoin_wallet_a_SOURCES = \
  db.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  $(DYNAMICCOIN_CORE_H)

# crypto primitives library
crypto_libdynamiccoin_crypto_a_CPPFLAGS = $(DYNAMICCOIN_CONFIG_INCLUDES)
crypto_libdynamiccoin_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/common.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/sha1.h \
  crypto/ripemd160.h

# univalue JSON library
univalue_libdynamiccoin_univalue_a_SOURCES = \
  univalue/univalue.cpp \
  univalue/univalue_read.cpp \
  univalue/univalue_write.cpp \
  univalue/univalue_escapes.h \
  univalue/univalue.h

# common: shared between dynamiccoind, and dynamiccoin-qt and non-server tools
libdynamiccoin_common_a_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
libdynamiccoin_common_a_SOURCES = \
  allocators.cpp \
  amount.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  core_read.cpp \
  core_write.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
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
  $(DYNAMICCOIN_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libdynamiccoin_util_a_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
libdynamiccoin_util_a_SOURCES = \
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
  $(DYNAMICCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libdynamiccoin_util_a_SOURCES += compat/glibc_compat.cpp
libdynamiccoin_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

# cli: shared between dynamiccoin-cli and dynamiccoin-qt
libdynamiccoin_cli_a_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
libdynamiccoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(DYNAMICCOIN_CORE_H)

nodist_libdynamiccoin_util_a_SOURCES = $(srcdir)/obj/build.h
#

# dynamiccoind binary #
dynamiccoind_LDADD = \
  $(LIBDYNAMICCOIN_SERVER) \
  $(LIBDYNAMICCOIN_COMMON) \
  $(LIBDYNAMICCOIN_UNIVALUE) \
  $(LIBDYNAMICCOIN_UTIL) \
  $(LIBDYNAMICCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

if ENABLE_WALLET
dynamiccoind_LDADD += libdynamiccoin_wallet.a
endif
dynamiccoind_SOURCES = dynamiccoind.cpp
#

if TARGET_WINDOWS
dynamiccoind_SOURCES += dynamiccoind-res.rc
endif

dynamiccoind_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS)
dynamiccoind_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
dynamiccoind_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

# dynamiccoin-cli binary #
dynamiccoin_cli_LDADD = \
  $(LIBDYNAMICCOIN_CLI) \
  $(LIBDYNAMICCOIN_UTIL) \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS)

dynamiccoin_cli_SOURCES = \
  dynamiccoin-cli.cpp

dynamiccoin_cli_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
#

# dynamiccoin-tx binary #
dynamiccoin_tx_LDADD = \
  $(LIBDYNAMICCOIN_UNIVALUE) \
  $(LIBDYNAMICCOIN_COMMON) \
  $(LIBDYNAMICCOIN_UTIL) \
  $(LIBDYNAMICCOIN_CRYPTO) \
  $(LIBSECP256K1) \
  $(BOOST_LIBS) \
  $(CRYPTO_LIBS)

dynamiccoin_tx_SOURCES = dynamiccoin-tx.cpp
dynamiccoin_tx_CPPFLAGS = $(DYNAMICCOIN_INCLUDES)
#
dynamiccoin_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
dynamiccoin_cli_SOURCES += dynamiccoin-cli-res.rc
endif
dynamiccoin_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if BUILD_DYNAMICCOIN_LIBS
include_HEADERS = script/bitcoinconsensus.h
libbitcoinconsensus_la_SOURCES = \
  primitives/transaction.cpp \
  crypto/hmac_sha512.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/ripemd160.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
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

libbitcoinconsensus_la_LDFLAGS = -no-undefined $(RELDFLAGS)
libbitcoinconsensus_la_LIBADD = $(CRYPTO_LIBS)
libbitcoinconsensus_la_CPPFLAGS = $(CRYPTO_CFLAGS) -I$(builddir)/obj -DBUILD_DYNAMICCOIN_INTERNAL
if USE_LIBSECP256K1
libbitcoinconsensus_la_LIBADD += secp256k1/libsecp256k1.la
endif
endif

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
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
