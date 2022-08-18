// Copyright (c) 2012-2014 The Bitcoin developers
// Copyright (c) 2022 The Dystater developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DYSTATER_VERSION_H
#define DYSTATER_VERSION_H

#include <string>

// // These need to be macros, as version.cpp's and bitcoin-qt.rc's voodoo requires it
#define CLIENT_VERSION_MAJOR       0
#define CLIENT_VERSION_MINOR       2
#define CLIENT_VERSION_REVISION    0
#define CLIENT_VERSION_BUILD       1

static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
// network protocol versioning
//

static const int PROTOCOL_VERSION = 20000;

// earlier versions not supported as of Feb 2012, and are disconnected
static const int MIN_PROTO_VERSION = 10000;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 10000;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 3200;
static const int NOBLKS_VERSION_END = 3240;
1
// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 20000;

// Converts the parameter X to a string after macro replacement on X has been performed.
// Don't merge these into one macro!
#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#endif
