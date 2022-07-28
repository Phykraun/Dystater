// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2021-2022 The Dystater developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DYSTATER_PRIMITIVES_TRANSACTION_H
#define DYSTATER_PRIMITIVES_TRANSACTION_H

#include "amount.h"
#include "script/script.h"
#include "serialize.h"
#include "uint256.h"

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }
    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { hash = 0; n = (unsigned int) -1; }
    bool IsNull() const { return (hash == 0 && n == (unsigned int) -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        return strprintf("COutPoint(%s, %d)", hash.ToString().c_str(), n);
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};




/** An input of a transaction.  It contains the location of the previous
 * transaction's output that it claims and a signature that matches the
 * output's public key.
 */
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    unsigned int nSequence;

    CTxIn()
    {
        nSequence = std::numeric_limits<unsigned int>::max();
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = prevoutIn;
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = COutPoint(hashPrevTx, nOut);
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    )

    bool IsFinal() const
    {
        return (nSequence == std::numeric_limits<unsigned int>::max());
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        std::string str;
        str += "CTxIn(";
        str += prevout.ToString();
        if (prevout.IsNull())
            str += strprintf(", coinbase %s", HexStr(scriptSig).c_str());
        else
            str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24).c_str());
        if (nSequence != std::numeric_limits<unsigned int>::max())
            str += strprintf(", nSequence=%u", nSequence);
        str += ")";
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};




/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
    int64 nValue;
    CScript scriptPubKey;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(int64 nValueIn, CScript scriptPubKeyIn)
    {
        nValue = nValueIn;
        scriptPubKey = scriptPubKeyIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    )

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
    }

    bool IsNull() const
    {
        return (nValue == -1);
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    bool IsDust() const;

    std::string ToString() const
    {
        if (scriptPubKey.size() < 6)
            return "CTxOut(error)";
        return strprintf("CTxOut(nValue=%"PRI64d".%08"PRI64d", scriptPubKey=%s)", nValue / COIN, nValue % COIN, scriptPubKey.ToString().substr(0,30).c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};



/** The basic transaction that is broadcasted on the network and contained in
 * blocks. A transaction can contain multiple inputs and outputs.
 */
class CTransaction
{
public:
    static int64 nMinTxFee;
    static int64 nMinRelayTxFee;
    static const int CURRENT_VERSION=1;
    int nVersion;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    unsigned int nLockTime;

    CTransaction()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
    )

    void SetNull()
    {
        nVersion = CTransaction::CURRENT_VERSION;
        vin.clear();
        vout.clear();
        nLockTime = 0;
    }

    bool IsNull() const
    {
        return (vin.empty() && vout.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    bool IsFinal(int nBlockHeight=0, int64 nBlockTime=0) const
    {
        // Time based nLockTime implemented in 0.1.6
        if (nLockTime == 0)
            return true;
        if (nBlockHeight == 0)
            nBlockHeight = nBestHeight;
        if (nBlockTime == 0)
            nBlockTime = GetAdjustedTime();
        if ((int64)nLockTime < ((int64)nLockTime < LOCKTIME_THRESHOLD ? (int64)nBlockHeight : nBlockTime))
            return true;
        BOOST_FOREACH(const CTxIn& txin, vin)
            if (!txin.IsFinal())
                return false;
        return true;
    }

    bool IsNewerThan(const CTransaction& old) const
    {
        if (vin.size() != old.vin.size())
            return false;
        for (unsigned int i = 0; i < vin.size(); i++)
            if (vin[i].prevout != old.vin[i].prevout)
                return false;

        bool fNewer = false;
        unsigned int nLowest = std::numeric_limits<unsigned int>::max();
        for (unsigned int i = 0; i < vin.size(); i++)
        {
            if (vin[i].nSequence != old.vin[i].nSequence)
            {
                if (vin[i].nSequence <= nLowest)
                {
                    fNewer = false;
                    nLowest = vin[i].nSequence;
                }
                if (old.vin[i].nSequence < nLowest)
                {
                    fNewer = true;
                    nLowest = old.vin[i].nSequence;
                }
            }
        }
        return fNewer;
    }

    bool IsCoinBase() const
    {
        return (vin.size() == 1 && vin[0].prevout.IsNull());
    }

    /** Check for standard transaction types
        @return True if all outputs (scriptPubKeys) use only standard transaction forms
    */
    bool IsStandard(std::string& strReason) const;
    bool IsStandard() const
    {
        std::string strReason;
        return IsStandard(strReason);
    }

    /** Check for standard transaction types
        @param[in] mapInputs	Map of previous transactions that have outputs we're spending
        @return True if all inputs (scriptSigs) use only standard transaction forms
    */
    bool AreInputsStandard(CCoinsViewCache& mapInputs) const;

    /** Count ECDSA signature operations the old-fashioned (pre-0.6) way
        @return number of sigops this transaction's outputs will produce when spent
    */
    unsigned int GetLegacySigOpCount() const;

    /** Count ECDSA signature operations in pay-to-script-hash inputs.

        @param[in] mapInputs	Map of previous transactions that have outputs we're spending
        @return maximum number of sigops required to validate this transaction's inputs
        @see CTransaction::FetchInputs
     */
    unsigned int GetP2SHSigOpCount(CCoinsViewCache& mapInputs) const;

    /** Amount of dystaters spent by this transaction.
        @return sum of all outputs (note: does not include fees)
     */
    int64 GetValueOut() const
    {
        int64 nValueOut = 0;
        BOOST_FOREACH(const CTxOut& txout, vout)
        {
            nValueOut += txout.nValue;
            if (!MoneyRange(txout.nValue) || !MoneyRange(nValueOut))
                throw std::runtime_error("CTransaction::GetValueOut() : value out of range");
        }
        return nValueOut;
    }

    /** Amount of dystaters coming in to this transaction
        Note that lightweight clients may not know anything besides the hash of previous transactions,
        so may not be able to calculate this.

        @param[in] mapInputs	Map of previous transactions that have outputs we're spending
        @return	Sum of value of all inputs (scriptSigs)
     */
    int64 GetValueIn(CCoinsViewCache& mapInputs) const;

    static bool AllowFree(double dPriority)
    {
        // Large (in bytes) low-priority (new, small-coin) transactions
        // need a fee.
        return dPriority > COIN * 144 / 250;
    }

    int64 GetMinFee(unsigned int nBlockSize=1, bool fAllowFree=true, enum GetMinFee_mode mode=GMF_BLOCK) const;

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return (a.nVersion  == b.nVersion &&
                a.vin       == b.vin &&
                a.vout      == b.vout &&
                a.nLockTime == b.nLockTime);
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }


    std::string ToString() const
    {
        std::string str;
        str += strprintf("CTransaction(hash=%s, ver=%d, vin.size=%"PRIszu", vout.size=%"PRIszu", nLockTime=%u)\n",
            GetHash().ToString().c_str(),
            nVersion,
            vin.size(),
            vout.size(),
            nLockTime);
        for (unsigned int i = 0; i < vin.size(); i++)
            str += "    " + vin[i].ToString() + "\n";
        for (unsigned int i = 0; i < vout.size(); i++)
            str += "    " + vout[i].ToString() + "\n";
        return str;
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }


    // Check whether all prevouts of this transaction are present in the UTXO set represented by view
    bool HaveInputs(CCoinsViewCache &view) const;

    // Check whether all inputs of this transaction are valid (no double spends, scripts & sigs, amounts)
    // This does not modify the UTXO set. If pvChecks is not NULL, script checks are pushed onto it
    // instead of being performed inline.
    bool CheckInputs(CValidationState &state, CCoinsViewCache &view, bool fScriptChecks = true,
                     unsigned int flags = SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC,
                     std::vector<CScriptCheck> *pvChecks = NULL) const;
    
    // Apply the effects of this transaction on the UTXO set represented by view
    void UpdateCoins(CValidationState &state, CCoinsViewCache &view, CTxUndo &txundo, int nHeight, const uint256 &txhash) const;

    // Context-independent validity checks
    bool CheckTransaction(CValidationState &state) const;

    // Try to accept this transaction into the memory pool
    bool AcceptToMemoryPool(CValidationState &state, bool fCheckInputs=true, bool fLimitFree = true, bool* pfMissingInputs=NULL);

protected:
    static const CTxOut &GetOutputFor(const CTxIn& input, CCoinsViewCache& mapInputs);
};
