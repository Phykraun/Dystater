DIST_SUBDIRS = secp256k1 univalue
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

DYSTATER_CONFIG_INCLUDES=-I$(builddir)/config

DYSTATER_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

DYSTATER_INCLUDES += -I$(srcdir)/secp256k1/include
DYSTATER_INCLUDES += $(UNIVALUE_CFLAGS)


LIBDYSTATER_SERVER=libdystater_server.a
LIBDYSTATER_WALLET=libdystater_wallet.a
LIBDYSTATER_COMMON=libdystater_common.a
LIBDYSTATER_CLI=libdystater_cli.a
LIBDYSTATER_UTIL=libdystater_util.a
LIBDYSTATER_CRYPTO=crypto/libdystater_crypto.a
LIBDYSTATERQT=qt/libdystaterqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*.h) $(wildcard secp256k1/src/*.c) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES = \
  crypto/libdystater_crypto.a \
  libdystater_util.a \
  libdystater_common.a \
  libdystater_server.a \
  libdystater_cli.a
if ENABLE_WALLET
DYSTATER_INCLUDES += $(BDB_CPPFLAGS)
EXTRA_LIBRARIES += libdystater_wallet.a
endif

if BUILD_DYSTATER_LIBS
lib_LTLIBRARIES = libdystaterconsensus.la
LIBDYSTATER_CONSENSUS=libdystaterconsensus.la
else
LIBDYSTATER_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_DYSTATERD
  bin_PROGRAMS += dystaterd
endif

if BUILD_DYSTATER_UTILS
  bin_PROGRAMS += dystater-cli dystater-tx
endif

.PHONY: FORCE
# dystater core #
DYSTATER_CORE_H = \
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
  support/cleanse.h \
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
libdystater_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between dystaterd and dystater-qt
libdystater_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libdystater_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
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
  $(DYSTATER_CORE_H)

# wallet: shared between dystaterd and dystater-qt, but only linked
# when wallet enabled
libdystater_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
libdystater_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_wallet_a_SOURCES = \
  db.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  $(DYSTATER_CORE_H)

# crypto primitives library
crypto_libdystater_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_CONFIG_INCLUDES)
crypto_libdystater_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
crypto_libdystater_crypto_a_SOURCES = \
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

# consensus: shared between all executables that validate any consensus rules.
libdystater_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
libdystater_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_consensus_a_SOURCES = \
  primitives/transaction.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  pubkey.cpp \
  script/script.cpp \
  script/interpreter.cpp \
  script/dystaterconsensus.cpp \
  uint256.cpp \
  utilstrencodings.cpp

# common: shared between dystaterd, and dystater-qt and non-server tools
libdystater_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
libdystater_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_common_a_SOURCES = \
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
  $(DYSTATER_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libdystater_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
libdystater_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_util_a_SOURCES = \
  compat/strnlen.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  random.cpp \
  rpcprotocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilstrencodings.cpp \
  utilmoneystr.cpp \
  utiltime.cpp \
  $(DYSTATER_CORE_H)

if GLIBC_BACK_COMPAT
libdystater_util_a_SOURCES += compat/glibc_compat.cpp
libdystater_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

# cli: shared between dystater-cli and dystater-qt
libdystater_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
libdystater_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
libdystater_cli_a_SOURCES = \
  rpcclient.cpp \
  $(DYSTATER_CORE_H)

nodist_libdystater_util_a_SOURCES = $(srcdir)/obj/build.h
#

# dystaterd binary #
dystaterd_SOURCES = dystaterd.cpp
#
dystaterd_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
dystaterd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
dystaterd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
dystaterd_SOURCES += dystaterd-res.rc
endif

dystaterd_LDADD = \
  $(LIBDYSTATER_SERVER) \
  $(LIBDYSTATER_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBDYSTATER_UTIL) \
  $(LIBDYSTATER_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBLEVELDB_SSE42) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

if ENABLE_WALLET
dystaterd_LDADD += libdystater_wallet.a
endif

dystaterd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS)

# dystater-cli binary #
dystater_cli_SOURCES = dystater-cli.cpp
dystater_cli_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES) $(EVENT_CFLAGS)
dystater_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
dystater_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
dystater_cli_SOURCES += dystater-cli-res.rc
endif

dystater_cli_LDADD = \
  $(LIBDYSTATER_CLI) \
  $(LIBUNIVALUE) \
  $(LIBDYSTATER_UTIL) \
  $(LIBDYSTATER_CRYPTO)

dystater_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# dystater-tx binary #
dystater_tx_SOURCES = dystater-tx.cpp
dystater_tx_CPPFLAGS = $(AM_CPPFLAGS) $(DYSTATER_INCLUDES)
dystater_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
dystater_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
dystater_tx_SOURCES += dystater-tx-res.rc
endif

dystater_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBDYSTATER_COMMON) \
  $(LIBDYSTATER_UTIL) \
  $(LIBDYSTATER_CONSENSUS) \
  $(LIBDYSTATER_CRYPTO) \
  $(LIBSECP256K1)

dystater_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)



if BUILD_DYSTATER_LIBS
include_HEADERS = script/dystaterconsensus.h
libdystaterconsensus_la_SOURCES = $(crypto_libdystater_crypto_a_SOURCES) $(libdystater_consensus_a_SOURCES)

if GLIBC_BACK_COMPAT
  libdystaterconsensus_la_SOURCES += compat/glibc_compat.cpp
  libdystaterconsensus_la_SOURCES += compat/glibcxx_compat.cpp
endif

libdystaterconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libdystaterconsensus_la_LIBADD = $(LIBSECP256K1)
libdystaterconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_DYSTATER_INTERNAL
libdystaterconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)

endif

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno
CLEANFILES += $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno

CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno


DISTCLEANFILES = obj/build.h

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS) $(CXXFLAGS) -c -o $@ $<

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.crc32c.include
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
