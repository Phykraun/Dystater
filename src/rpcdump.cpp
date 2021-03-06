

#include "init.h" // for pwalletMain
#include "dystaterrpc.h"
#include "ui_interface.h"

#include <boost/lexical_cast.hpp>

#include "json_spirit_v4.08/json_spirit/json_spirit_reader_template.h"
#include "json_spirit_v4.08/json_spirit/json_spirit_writer_template.h"
#include "json_spirit_v4.08/json_spirit/json_spirit_utils.h"

#define printf OutputDebugStringF

// using namespace boost::asio;
using namespace json_spirit;
using namespace std;

extern Object JSONRPCError(int code, const string& message);

class CTxDump
{
public:
    CBlockIndex *pindex;
    int64 nValue;
    bool fSpent;
    CWalletTx* ptx;
    int nOut;
    CTxDump(CWalletTx* ptx = NULL, int nOut = -1)
    {
        pindex = NULL;
        nValue = 0;
        fSpent = false;
        this->ptx = ptx;
        this->nOut = nOut;
    }
};

Value importprivkey(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "importprivkey <dystaterprivkey> [label]\n"
            "Adds a private key (as returned by dumpprivkey) to your wallet.");
    
    string strSecret = params[0].get_str();
    string strLabel = "";
    if (params.size() > 1)
        strLabel = params[1].get_str();
    CDystaterSecret vchSecret;
    bool fGood = vchSecret.SetString(strSecret);

    if (!fGood) throw JSONRPCError(-5,"Invalid private key");

    CKey key;
    bool fCompressed;
    CSecret secret = vchSecret.GetSecret(fCompressed);
    key.SetSecret(secret, fCompressed);
    CDystaterAddress vchAddress = CDystaterAddress(key.GetPubKey());

    {
        LOCK2(cs_main, pwalletMain->cs_wallet);

        pwalletMain->MarkDirty();
        pwalletMain->SetAddressBookName(vchAddress, strLabel);

        if (!pwalletMain->AddKey(key))
            throw JSONRPCError(-4,"Error adding key to wallet");

        pwalletMain->ScanForWalletTransactions(pindexGenesisBlock, true);
        pwalletMain->ReacceptWalletTransactions();
    }

    MainFrameRepaint();

    return Value::null;
}

Value dumpprivkey(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "dumpprivkey <dystateraddress>\n"
            "Reveals the private key corresponding to <dystateraddress>.");
    
    string strAddress = params[0].get_str();
    CDystaterAddress address;
    if (!address.SetString(strAddress))
        throw JSONRPCError(-5, "Invalid dystater address");
    CSecret vchSecret;
    bool fCompressed;
    if (!pwalletMain->GetSecret(address, vchSecret, fCompressed))
        throw JSONRPCError(-4,"Private key for address " + strAddress + " is not known");
    return CDystaterSecret(vchSecret, fCompressed).ToString();
}
