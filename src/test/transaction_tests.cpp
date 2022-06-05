// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2021-2022 The Dystater developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include "../main.h"
#include "../wallet.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(transaction_tests)

BOOST_AUTO_TEST_CASE(basic_transaction_tests)
{
    // Random real transaction 

    unsigned char ch[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0xe6, 0x1b, 0x34, 0x3d, 0x62, 0x7f, 0xe0, 0x81, 0xd5, 0xfa, 0x45, 0x3b, 0x48, 0x26, 0x1c, 0xc4, 0xed, 0x05, 0x98, 0xb8, 0x42, 0xba, 0x7a, 0xc3, 0x79, 0x6d, 0x28, 0xc0, 0x14, 0x3f, 0x82, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x47, 0x30, 0x45, 0x02, 0x20, 0x5c, 0x1f, 0xf7, 0xd7, 0x8a, 0x11, 0x56, 0xac, 0x9d, 0xf8, 0xf3, 0xc9, 0x3f, 0x8a, 0x3e, 0xf8, 0x2c, 0x78, 0x44, 0x90, 0xde, 0x70, 0x28, 0x28, 0xd5, 0x38, 0xc3, 0x1c, 0x64, 0x1e, 0x32, 0xb0, 0x22, 0x12, 0x87, 0x68, 0xf3, 0x90, 0x75, 0x81, 0xb1, 0x24, 0x93, 0xb4, 0x7c, 0xb5, 0x3a, 0x6a, 0xe1, 0x54, 0xad, 0x21, 0xf7, 0xb6, 0x09, 0x30, 0x5b, 0x4b, 0xd2, 0x07, 0x36, 0x68, 0x12, 0x47, 0x62, 0xfL, 0x01, 0x41, 0x04, 0x02, 0xc6, 0xb0, 0x04, 0x7b, 0xb0, 0x6b, 0x4e, 0xc8, 0x9d, 0xa5, 0x6c, 0x05, 0xbf, 0x87, 0xd3, 0x6d, 0xb3, 0x86, 0x13, 0x29, 0x10, 0xc4, 0x97, 0xc3, 0x5f, 0xca, 0x85, 0xaf, 0x43, 0x7c, 0x72, 0x14, 0x7e, 0x44, 0x37, 0xa7, 0x89, 0xc8, 0x5b, 0xc5, 0x08, 0x67, 0x3b, 0x1b, 0xe9, 0x7a, 0x92, 0xe7, 0xac, 0x86, 0xce, 0xe3, 0x53, 0x08, 0x83, 0x38, 0x29, 0x98, 0xc7, 0x79, 0xad, 0x09, 0x65, 0xff, 0xff, 0xff, 0xff, 0x02, 0x00, 0xe2, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0x5c, 0x44, 0x91, 0x2c, 0xc4, 0x02, 0x7d, 0xef, 0x91, 0x2d, 0x33, 0x43, 0x05, 0x64, 0xcc, 0x32, 0x60, 0xe3, 0x22, 0x21, 0x88, 0xac, 0x20, 0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x76, 0xa9, 0x14, 0xad, 0x51, 0xce, 0xce, 0x17, 0x3e, 0x01, 0xc3, 0xd8, 0x79, 0xfc, 0x84, 0x4d, 0xc4, 0xa5, 0xe4, 0xd7, 0x1a, 0x31, 0x71, 0x88, 0xac, 0x00, 0x00, 0x00, 0x00, 0x00};

    vector<unsigned char> vch(ch, ch + sizeof(ch) -1);
    CDataStream stream(vch, SER_DISK, CLIENT_VERSION);
    CTransaction tx;
    stream >> tx;
    BOOST_CHECK_MESSAGE(tx.CheckTransaction(), "Simple deserialized transaction should be valid.");

    // Check that duplicate txins fail
    tx.vin.push_back(tx.vin[0]);
    BOOST_CHECK_MESSAGE(!tx.CheckTransaction(), "Transaction with duplicate txins should be invalid.");
}

//
// Helper: create two dummy transactions, each with
// two outputs.  The first has 11 and 50 CENT outputs
// paid to a TX_PUBKEY, the second 21 and 22 CENT outputs
// paid to a TX_PUBKEYHASH.
//
static std::vector<CTransaction>
SetupDummyInputs(CBasicKeyStore& keystoreRet, MapPrevTx& inputsRet)
{
    std::vector<CTransaction> dummyTransactions;
    dummyTransactions.resize(2);

    // Add some keys to the keystore:
    CKey key[4];
    for (int i = 0; i < 4; i++)
    {
        key[i].MakeNewKey(i % 2);
        keystoreRet.AddKey(key[i]);
    }

    // Create some dummy input transactions
    dummyTransactions[0].vout.resize(2);
    dummyTransactions[0].vout[0].nValue = 11*CENT;
    dummyTransactions[0].vout[0].scriptPubKey << key[0].GetPubKey() << OP_CHECKSIG;
    dummyTransactions[0].vout[1].nValue = 50*CENT;
    dummyTransactions[0].vout[1].scriptPubKey << key[1].GetPubKey() << OP_CHECKSIG;
    inputsRet[dummyTransactions[0].GetHash()] = make_pair(CTxIndex(), dummyTransactions[0]);

    dummyTransactions[1].vout.resize(2);
    dummyTransactions[1].vout[0].nValue = 21*CENT;
    dummyTransactions[1].vout[0].scriptPubKey.SetDystaterAddress(key[2].GetPubKey());
    dummyTransactions[1].vout[1].nValue = 22*CENT;
    dummyTransactions[1].vout[1].scriptPubKey.SetDystaterAddress(key[3].GetPubKey());
    inputsRet[dummyTransactions[1].GetHash()] = make_pair(CTxIndex(), dummyTransactions[1]);

    return dummyTransactions;
}

BOOST_AUTO_TEST_CASE(test_Get)
{
    CBasicKeyStore keystore;
    MapPrevTx dummyInputs;
    std::vector<CTransaction> dummyTransactions = SetupDummyInputs(keystore, dummyInputs);

    CTransaction t1;
    t1.vin.resize(3);
    t1.vin[0].prevout.hash = dummyTransactions[0].GetHash();
    t1.vin[0].prevout.n = 1;
    t1.vin[0].scriptSig << std::vector<unsigned char>(65, 0);
    t1.vin[1].prevout.hash = dummyTransactions[1].GetHash();
    t1.vin[1].prevout.n = 0;
    t1.vin[1].scriptSig << std::vector<unsigned char>(65, 0) << std::vector<unsigned char>(33, 4);
    t1.vin[2].prevout.hash = dummyTransactions[1].GetHash();
    t1.vin[2].prevout.n = 1;
    t1.vin[2].scriptSig << std::vector<unsigned char>(65, 0) << std::vector<unsigned char>(33, 4);
    t1.vout.resize(2);
    t1.vout[0].nValue = 90*CENT;
    t1.vout[0].scriptPubKey << OP_1;

    BOOST_CHECK(t1.AreInputsStandard(dummyInputs));
    BOOST_CHECK_EQUAL(t1.GetValueIn(dummyInputs), (50+21+22)*CENT);

    // Adding extra junk to the scriptSig should make it non-standard:
    t1.vin[0].scriptSig << OP_11;
    BOOST_CHECK(!t1.AreInputsStandard(dummyInputs));

    // ... as should not having enough:
    t1.vin[0].scriptSig = CScript();
    BOOST_CHECK(!t1.AreInputsStandard(dummyInputs));
}

BOOST_AUTO_TEST_CASE(test_GetThrow)
{
    CBasicKeyStore keystore;
    MapPrevTx dummyInputs;
    std::vector<CTransaction> dummyTransactions = SetupDummyInputs(keystore, dummyInputs);

    MapPrevTx missingInputs;

    CTransaction t1;
    t1.vin.resize(3);
    t1.vin[0].prevout.hash = dummyTransactions[0].GetHash();
    t1.vin[0].prevout.n = 0;
    t1.vin[1].prevout.hash = dummyTransactions[1].GetHash();;
    t1.vin[1].prevout.n = 0;
    t1.vin[2].prevout.hash = dummyTransactions[1].GetHash();;
    t1.vin[2].prevout.n = 1;
    t1.vout.resize(2);
    t1.vout[0].nValue = 90*CENT;
    t1.vout[0].scriptPubKey << OP_1;

    BOOST_CHECK_THROW(t1.AreInputsStandard(missingInputs), runtime_error);
    BOOST_CHECK_THROW(t1.GetValueIn(missingInputs), runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
