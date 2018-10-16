// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "checkpoints.h"
#include "clientversion.h"
#include "contractconfig.h"

#include "main.h"
#include "rpcserver.h"
#include "sync.h"
#include "txdb.h"
#include "util.h"
#include "utilmoneystr.h"

#include <stdint.h>
#include <univalue.h>
#include <string>

using namespace std;

extern void TxToJSON(const CTransaction& tx, const uint256 hashBlock, UniValue& entry);
void ScriptPubKeyToJSON(const CScript& scriptPubKey, UniValue& out, bool fIncludeHex);

double GetDifficulty(const CBlockIndex* blockindex)
{
    // Floating point number that is a multiple of the minimum difficulty,
    // minimum difficulty = 1.0.
    if (blockindex == NULL) {
        if (chainActive.Tip() == NULL)
            return 1.0;
        else
            blockindex = chainActive.Tip();
    }

    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29) {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29) {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

#ifdef  POW_IN_POS_PHASE

double GetDifficulty2(const CBlockIndex* blockindex)
{
    // Floating point number that is a multiple of the minimum difficulty,
    // minimum difficulty = 1.0.
    if (blockindex == NULL) {
        if (chainActive.Tip() == NULL)
            return 1.0;
        else
            blockindex = chainActive.Tip();
    }

    int nShift = (blockindex->nBits2 >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits2 & 0x00ffffff);

    while (nShift < 29) {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29) {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

#endif

UniValue blockToJSON(const CBlock& block, const CBlockIndex* blockindex, bool txDetails = false)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("hash", block.GetHash().GetHex()));
    int confirmations = -1;
    // Only report confirmations if the block is on the main chain
    if (chainActive.Contains(blockindex))
        confirmations = chainActive.Height() - blockindex->nHeight + 1;
    result.push_back(Pair("confirmations", confirmations));
    result.push_back(Pair("size", (int)::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION)));
    result.push_back(Pair("height", blockindex->nHeight));
    result.push_back(Pair("version", block.nVersion));
    result.push_back(Pair("merkleroot", block.hashMerkleRoot.GetHex()));
    result.push_back(Pair("acc_checkpoint", block.nAccumulatorCheckpoint.GetHex()));
    UniValue txs(UniValue::VARR);
    BOOST_FOREACH (const CTransaction& tx, block.vtx) {
        if (txDetails) {
            UniValue objTx(UniValue::VOBJ);
            TxToJSON(tx, uint256(0), objTx);
            txs.push_back(objTx);
        } else
            txs.push_back(tx.GetHash().GetHex());
    }
    result.push_back(Pair("tx", txs));
    result.push_back(Pair("time", block.GetBlockTime()));
    result.push_back(Pair("nonce", (uint64_t)block.nNonce));
    result.push_back(Pair("bits", strprintf("%08x", block.nBits)));
    result.push_back(Pair("difficulty", GetDifficulty(blockindex)));
    result.push_back(Pair("chainwork", blockindex->nChainWork.GetHex()));

    if (blockindex->pprev)
        result.push_back(Pair("previousblockhash", blockindex->pprev->GetBlockHash().GetHex()));
    CBlockIndex* pnext = chainActive.Next(blockindex);
    if (pnext)
        result.push_back(Pair("nextblockhash", pnext->GetBlockHash().GetHex()));

    result.push_back(Pair("moneysupply",ValueFromAmount(blockindex->nMoneySupply)));

    UniValue zuloObj(UniValue::VOBJ);
    for (auto denom : libzerocoin::zerocoinDenomList) {
        zuloObj.push_back(Pair(to_string(denom), ValueFromAmount(blockindex->mapZerocoinSupply.at(denom) * (denom*COIN))));
    }
    zuloObj.push_back(Pair("total", ValueFromAmount(blockindex->GetZerocoinSupply())));
    result.push_back(Pair("zULOsupply", zuloObj));

    return result;
}


UniValue blockHeaderToJSON(const CBlock& block, const CBlockIndex* blockindex)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("version", block.nVersion));
    if (blockindex->pprev)
        result.push_back(Pair("previousblockhash", blockindex->pprev->GetBlockHash().GetHex()));
    result.push_back(Pair("merkleroot", block.hashMerkleRoot.GetHex()));
    result.push_back(Pair("time", block.GetBlockTime()));
    result.push_back(Pair("bits", strprintf("%08x", block.nBits)));
    result.push_back(Pair("nonce", (uint64_t)block.nNonce));
    return result;
}


UniValue getblockcount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getblockcount\n"
            "\nReturns the number of blocks in the longest block chain.\n"
            "\nResult:\n"
            "n    (numeric) The current block count\n"
            "\nExamples:\n" +
            HelpExampleCli("getblockcount", "") + HelpExampleRpc("getblockcount", ""));

    return chainActive.Height();
}

UniValue getbestblockhash(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getbestblockhash\n"
            "\nReturns the hash of the best (tip) block in the longest block chain.\n"
            "\nResult\n"
            "\"hex\"      (string) the block hash hex encoded\n"
            "\nExamples\n" +
            HelpExampleCli("getbestblockhash", "") + HelpExampleRpc("getbestblockhash", ""));

    return chainActive.Tip()->GetBlockHash().GetHex();
}

UniValue getdifficulty(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getdifficulty\n"
            "\nReturns the proof-of-work difficulty as a multiple of the minimum difficulty.\n"
            "\nResult:\n"
            "n.nnn       (numeric) the proof-of-work difficulty as a multiple of the minimum difficulty.\n"
            "\nExamples:\n" +
            HelpExampleCli("getdifficulty", "") + HelpExampleRpc("getdifficulty", ""));

    return GetDifficulty();
}


UniValue getrawmempool(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getrawmempool ( verbose )\n"
            "\nReturns all transaction ids in memory pool as a json array of string transaction ids.\n"
            "\nArguments:\n"
            "1. verbose           (boolean, optional, default=false) true for a json object, false for array of transaction ids\n"
            "\nResult: (for verbose = false):\n"
            "[                     (json array of string)\n"
            "  \"transactionid\"     (string) The transaction id\n"
            "  ,...\n"
            "]\n"
            "\nResult: (for verbose = true):\n"
            "{                           (json object)\n"
            "  \"transactionid\" : {       (json object)\n"
            "    \"size\" : n,             (numeric) transaction size in bytes\n"
            "    \"fee\" : n,              (numeric) transaction fee in eulo\n"
            "    \"time\" : n,             (numeric) local time transaction entered pool in seconds since 1 Jan 1970 GMT\n"
            "    \"height\" : n,           (numeric) block height when transaction entered pool\n"
            "    \"startingpriority\" : n, (numeric) priority when transaction entered pool\n"
            "    \"currentpriority\" : n,  (numeric) transaction priority now\n"
            "    \"depends\" : [           (array) unconfirmed transactions used as inputs for this transaction\n"
            "        \"transactionid\",    (string) parent transaction id\n"
            "       ... ]\n"
            "  }, ...\n"
            "]\n"
            "\nExamples\n" +
            HelpExampleCli("getrawmempool", "true") + HelpExampleRpc("getrawmempool", "true"));

    bool fVerbose = false;
    if (params.size() > 0)
        fVerbose = params[0].get_bool();

    if (fVerbose) {
        LOCK(mempool.cs);
        UniValue o(UniValue::VOBJ);
        BOOST_FOREACH (const PAIRTYPE(uint256, CTxMemPoolEntry) & entry, mempool.mapTx) {
            const uint256& hash = entry.first;
            const CTxMemPoolEntry& e = entry.second;
            UniValue info(UniValue::VOBJ);
            info.push_back(Pair("size", (int)e.GetTxSize()));
            info.push_back(Pair("fee", ValueFromAmount(e.GetFee())));
            info.push_back(Pair("time", e.GetTime()));
            info.push_back(Pair("height", (int)e.GetHeight()));
            info.push_back(Pair("startingpriority", e.GetPriority(e.GetHeight())));
            info.push_back(Pair("currentpriority", e.GetPriority(chainActive.Height())));
            const CTransaction& tx = e.GetTx();
            set<string> setDepends;
            BOOST_FOREACH (const CTxIn& txin, tx.vin) {
                if (mempool.exists(txin.prevout.hash))
                    setDepends.insert(txin.prevout.hash.ToString());
            }

            UniValue depends(UniValue::VARR);
            BOOST_FOREACH(const string& dep, setDepends) {
                depends.push_back(dep);
            }

            info.push_back(Pair("depends", depends));
            o.push_back(Pair(hash.ToString(), info));
        }
        return o;
    } else {
        vector<uint256> vtxid;
        mempool.queryHashes(vtxid);

        UniValue a(UniValue::VARR);
        BOOST_FOREACH (const uint256& hash, vtxid)
            a.push_back(hash.ToString());

        return a;
    }
}

UniValue getblockhash(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getblockhash index\n"
            "\nReturns hash of block in best-block-chain at index provided.\n"
            "\nArguments:\n"
            "1. index         (numeric, required) The block index\n"
            "\nResult:\n"
            "\"hash\"         (string) The block hash\n"
            "\nExamples:\n" +
            HelpExampleCli("getblockhash", "1000") + HelpExampleRpc("getblockhash", "1000"));

    int nHeight = params[0].get_int();
    if (nHeight < 0 || nHeight > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

    CBlockIndex* pblockindex = chainActive[nHeight];
    return pblockindex->GetBlockHash().GetHex();
}

//------------------cpoy from blockexplorer.cpp
inline std::string utostr(unsigned int n)
{
    return strprintf("%u", n);
}

static std::string makeHRef(const std::string& Str)
{
    return "<a href=\"" + Str + "\">" + Str + "</a>";
}

CTxOut getPrevOut2(const COutPoint& out)
{
    CTransaction tx;
    uint256 hashBlock;
    if (GetTransaction(out.hash, tx, hashBlock, true))
        return tx.vout[out.n];
    return CTxOut();
}

static CAmount getTxIn(const CTransaction& tx)
{
    if (tx.IsCoinBase())
        return 0;

    CAmount Sum = 0;
    for (unsigned int i = 0; i < tx.vin.size(); i++)
        Sum += getPrevOut2(tx.vin[i].prevout).nValue;
    return Sum;
}

static std::string ValueToString(CAmount nValue, bool AllowNegative = false)
{
    if (nValue < 0 && !AllowNegative)
        return "<span>unknown</span>";

    double value  = nValue*1.0/100000000;

    std::string Str = std::to_string(value);


    if (AllowNegative && nValue > 0)
        Str = '+' + Str;
    else if(AllowNegative && nValue < 0)
        Str = '-' + Str;


    return std::string("<span>") + Str + "</span>";
}

static std::string ValueToString2(CAmount nValue, bool AllowNegative = false)
{
    if (nValue < 0 && !AllowNegative)
        return "unknown";

    double value  = nValue*1.0/100000000;

    std::string Str = std::to_string(value);


    if (AllowNegative && nValue > 0)
        Str = '+' + Str;
    else if(AllowNegative && nValue < 0)
        Str = '-' + Str;


    return  Str;
}

string FormatScript2(const CScript& script)
{
    string ret;
    CScript::const_iterator it = script.begin();
    opcodetype op;
    while (it != script.end()) {
        CScript::const_iterator it2 = it;
        vector<unsigned char> vch;
        if (script.GetOp2(it, op, &vch)) {
            if (op == OP_0) {
                ret += "0 ";
                continue;
            } else if ((op >= OP_1 && op <= OP_16) || op == OP_1NEGATE) {
                ret += strprintf("%i ", op - OP_1NEGATE - 1);
                continue;
            } else if (op >= OP_NOP && op <= OP_CHECKMULTISIGVERIFY) {
                string str(GetOpName(op));
                if (str.substr(0, 3) == string("OP_")) {
                    ret += str.substr(3, string::npos) + " ";
                    continue;
                }
            }
            if (vch.size() > 0) {
                ret += strprintf("0x%x 0x%x ", HexStr(it2, it - vch.size()), HexStr(it - vch.size(), it));
            } else {
                ret += strprintf("0x%x", HexStr(it2, it));
            }
            continue;
        }
        ret += strprintf("0x%x ", HexStr(it2, script.end()));
        break;
    }
    return ret.substr(0, ret.size() - 1);
}

static std::string ScriptToString(const CScript& Script, bool Long = false, bool Highlight = false)
{
    if (Script.empty())
        return "unknown";

    CTxDestination Dest;
    CBitcoinAddress Address;
    if (ExtractDestination(Script, Dest) && Address.Set(Dest)) {
        if (Highlight)
            return "<span class=\"addr\">" + Address.ToString() + "</span>";
        else
            return makeHRef(Address.ToString());
    } else
        return Long ? "<pre>" + FormatScript2(Script) + "</pre>" : "Non-standard script";
}

static std::string ScriptToString2(const CScript& Script, bool Long = false, bool Highlight = false)
{
    if (Script.empty())
        return "unknown";

    CTxDestination Dest;
    CBitcoinAddress Address;
    if (ExtractDestination(Script, Dest) && Address.Set(Dest)) {
            return Address.ToString();
    } else
        return Long ? FormatScript2(Script): "Non-standard script";
}

static std::string TimeToString(uint64_t Time)
{
    time_t t;
    tm* local;
    char buf[128]= {0};

    t = (long int)Time;

    local = localtime(&t);
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", local);

    return buf;
}

static std::string makeHTMLTableRow(const std::string* pCells, int n)
{
    std::string Result = "<tr>";
    for (int i = 0; i < n; i++) {
        Result += "<td class=\"d" + utostr(i) + "\">";
        Result += pCells[i];
        Result += "</td>";
    }
    Result += "</tr>";
    return Result;
}

static const char* table = "<table>";

static std::string makeHTMLTable(const std::string* pCells, int nRows, int nColumns)
{
    std::string Table = table;
    for (int i = 0; i < nRows; i++)
        Table += makeHTMLTableRow(pCells + i * nColumns, nColumns);
    Table += "</table>";
    return Table;
}


static std::string TxToRow(const CTransaction& tx, const CScript& Highlight = CScript(), const std::string& Prepend = std::string(), int64_t* pSum = NULL)
{
    std::string InAmounts, InAddresses, OutAmounts, OutAddresses;
    int64_t Delta = 0;
    for (unsigned int j = 0; j < tx.vin.size(); j++) {
        if (tx.IsCoinBase()) {
            InAmounts += ValueToString(tx.GetValueOut());
            InAddresses += "coinbase";
        } else {
            CTxOut PrevOut = getPrevOut2(tx.vin[j].prevout);
            InAmounts += ValueToString(PrevOut.nValue);
            InAddresses += ScriptToString(PrevOut.scriptPubKey, false, PrevOut.scriptPubKey == Highlight).c_str();
            if (PrevOut.scriptPubKey == Highlight)
                Delta -= PrevOut.nValue;
        }
        if (j + 1 != tx.vin.size()) {
            InAmounts += "<br/>";
            InAddresses += "<br/>";
        }
    }
    for (unsigned int j = 0; j < tx.vout.size(); j++) {
        CTxOut Out = tx.vout[j];
        OutAmounts += ValueToString(Out.nValue);
        OutAddresses += ScriptToString(Out.scriptPubKey, false, Out.scriptPubKey == Highlight);
        if (Out.scriptPubKey == Highlight)
            Delta += Out.nValue;
        if (j + 1 != tx.vout.size()) {
            OutAmounts += "<br/>";
            OutAddresses += "<br/>";
        }
    }

    std::string List[8] =
        {
            Prepend,
            makeHRef(tx.GetHash().GetHex()),
            InAddresses,
            InAmounts,
            OutAddresses,
            OutAmounts,
            "",
            ""
    };

    int n = sizeof(List) / sizeof(std::string) - 2;

    if (!Highlight.empty()) {
        List[n++] = std::string("<font color=\"") + ((Delta > 0) ? "green" : "red") + "\">" + ValueToString(Delta, true) + "</font>";
        *pSum += Delta;
        List[n++] = ValueToString(*pSum);
        return makeHTMLTableRow(List, n);
    }
    return makeHTMLTableRow(List + 1, n - 1);
}

static UniValue TxToRow2(const CTransaction& tx, const CScript& Highlight = CScript(), const std::string& Prepend = std::string(), int64_t* pSum = NULL)
{
    UniValue info(UniValue::VOBJ);
    UniValue from_array(UniValue::VARR);
    UniValue to_array(UniValue::VARR);

    int64_t Delta = 0;
    for (unsigned int j = 0; j < tx.vin.size(); j++) {
        std::string InAmounts, InAddresses;
        if (tx.IsCoinBase()) {
            InAmounts = ValueToString2(tx.GetValueOut());
            InAddresses = "coinbase";
        } else {
            CTxOut PrevOut = getPrevOut2(tx.vin[j].prevout);
            InAmounts = ValueToString2(PrevOut.nValue);
            InAddresses = ScriptToString2(PrevOut.scriptPubKey, false, PrevOut.scriptPubKey == Highlight).c_str();
            if (PrevOut.scriptPubKey == Highlight)
                Delta -= PrevOut.nValue;
        }


        UniValue from_item(UniValue::VOBJ);
        from_item.push_back(Pair(InAddresses,InAmounts));

        from_array.push_back(from_item);

    }
    for (unsigned int j = 0; j < tx.vout.size(); j++) {

        std::string  OutAmounts, OutAddresses;


        CTxOut Out = tx.vout[j];
        OutAmounts = ValueToString2(Out.nValue);
        OutAddresses = ScriptToString2(Out.scriptPubKey, false, Out.scriptPubKey == Highlight);
        if (Out.scriptPubKey == Highlight)
            Delta += Out.nValue;

        UniValue to_item(UniValue::VOBJ);
        to_item.push_back(Pair(OutAddresses,OutAmounts));

        to_array.push_back(to_item);
    }


    info.push_back(Pair("txid", tx.GetHash().GetHex()));
    info.push_back(Pair("from_array",from_array ));
    info.push_back(Pair("to_array",to_array ));

    return info;
}

std::string getexplorerBlockHash2(int64_t Height)
{
    std::string genesisblockhash = "0000041e482b9b9691d98eefb48473405c0b8ec31b76df3797c74a78680ef818";
    CBlockIndex* pindexBest = mapBlockIndex[chainActive.Tip()->GetBlockHash()];
    if ((Height < 0) || (Height > pindexBest->nHeight)) {
        return genesisblockhash;
    }

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[chainActive.Tip()->GetBlockHash()];
    while (pblockindex->nHeight > Height)
        pblockindex = pblockindex->pprev;
    return pblockindex->GetBlockHash().GetHex(); // pblockindex->phashBlock->GetHex();
}


const CBlockIndex* getexplorerBlockIndex2(int64_t height)
{
    std::string hex = getexplorerBlockHash2(height);
    uint256 hash = uint256S(hex);
    return mapBlockIndex[hash];
}

UniValue BlocksToString(int64_t from,int64_t to)
{
    UniValue res(UniValue::VOBJ);

    for(int64_t i = from ;i <= to;i++)
    {
        const CBlockIndex* pBlock = getexplorerBlockIndex2(i);

        if(pBlock == nullptr)
            continue;


        CBlock block;
        ReadBlockFromDisk(block, pBlock);

        CAmount Fees = 0;
        CAmount OutVolume = 0;
        CAmount Reward = 0;

        for (unsigned int i = 0; i < block.vtx.size(); i++) {
            const CTransaction& tx = block.vtx[i];

            CAmount In = getTxIn(tx);
            CAmount Out = tx.GetValueOut();
            if (tx.IsCoinBase())
                Reward += Out;
            else if (In < 0)
                Fees = -Params().MaxMoneyOut();
            else {
                Fees += In - Out;
                OutVolume += Out;
            }
        }

        CAmount Generated;
        if (pBlock->nHeight == 0)
            Generated = OutVolume;
        else
            Generated = GetBlockValue(pBlock->nHeight - 1);


        UniValue info(UniValue::VOBJ);
        info.push_back(Pair("Timestamp", TimeToString(block.nTime)));
        info.push_back(Pair("Transactions", itostr(block.vtx.size())));
        info.push_back(Pair("Value", ValueToString2(OutVolume + Fees + Generated)));
        info.push_back(Pair("Difficulty", strprintf("%.4f", GetDifficulty(pBlock))));

        res.push_back(Pair(itostr(i), info));
    }


    return res;
}


UniValue BlockToString2(CBlockIndex* pBlock)
{


    if (!pBlock)
        return "";

    UniValue info(UniValue::VOBJ);
    UniValue tx_array(UniValue::VARR);

    CBlock block;
    ReadBlockFromDisk(block, pBlock);



    CAmount Fees = 0;
    CAmount OutVolume = 0;
    CAmount Reward = 0;

    std::string TxLabels[] = {"Hash", "From", "Amount", "To", "Amount"};

    std::string TxContent = table + makeHTMLTableRow(TxLabels, sizeof(TxLabels) / sizeof(std::string));
    for (unsigned int i = 0; i < block.vtx.size(); i++) {
        const CTransaction& tx = block.vtx[i];
        tx_array.push_back( TxToRow2(tx) );

        CAmount In = getTxIn(tx);
        CAmount Out = tx.GetValueOut();
        if (tx.IsCoinBase())
            Reward += Out;
        else if (In < 0)
            Fees = -Params().MaxMoneyOut();
        else {
            Fees += In - Out;
            OutVolume += Out;
        }
    }
    TxContent += "</table>";


    CAmount Generated;
    if (pBlock->nHeight == 0)
        Generated = OutVolume;
    else
        Generated = GetBlockValue(pBlock->nHeight - 1);


//    return TimeToString(block.nTime);
//    return strprintf("%.4f", GetDifficulty(pBlock));

    std::string BlockContentCells[] =
        {
            "Height", itostr(pBlock->nHeight),
            "Size", itostr(GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION)),
            "Number of Transactions", itostr(block.vtx.size()),
            "Value Out", ValueToString(OutVolume),
            "Fees", ValueToString(Fees),
            "Generated", ValueToString(Generated),
            "Timestamp", TimeToString(block.nTime),
            "Difficulty", strprintf("%.4f", GetDifficulty(pBlock)),
            "Bits", utostr(block.nBits),
            "Nonce", utostr(block.nNonce),
            "Version", itostr(block.nVersion),
            "Hash", "<pre>" + block.GetHash().GetHex() + "</pre>",
            "Merkle Root", "<pre>" + block.hashMerkleRoot.GetHex() + "</pre>",
//             _("Hash Whole Block"), "<pre>" + block.hashWholeBlock.GetHex() + "</pre>"
//             _("Miner Signature"), "<pre>" + block.MinerSignature.ToString() + "</pre>"
        };


    info.push_back(Pair("height",itostr(pBlock->nHeight)));
    info.push_back(Pair("size",itostr(GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION))));
    info.push_back(Pair("tx_num",itostr(block.vtx.size())));
    info.push_back(Pair("value_out",ValueToString2(OutVolume)));
    info.push_back(Pair("fees",ValueToString2(Fees)));
    info.push_back(Pair("generated",ValueToString2(Generated)));
    info.push_back(Pair("timestamp",TimeToString(block.nTime)));
    info.push_back(Pair("difficulty",strprintf("%.4f", GetDifficulty(pBlock))));
#ifdef  POW_IN_POS_PHASE
    if(pBlock->nHeight>Params().LAST_POW_BLOCK())
        info.push_back(Pair("difficulty2",strprintf("%.4f", GetDifficulty2(pBlock))));
#endif
    info.push_back(Pair("bits",utostr(block.nBits)));
    info.push_back(Pair("nonce",utostr(block.nNonce)));
    info.push_back(Pair("version",itostr(block.nVersion)));
    info.push_back(Pair("hash",block.GetHash().GetHex()));
    info.push_back(Pair("merkle_root",block.hashMerkleRoot.GetHex()));
    info.push_back(Pair("tx_array",tx_array));

    return info;
}


UniValue getblockhashexplorer(const UniValue& params, bool fHelp)
{
    UniValue result(UniValue::VOBJ);

    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getblockhashexplorer index\n"
            "\nReturns hash of block in best-block-chain at index provided.\n"
            "\nArguments:\n"
            "1. index         (numeric, required) The block index\n"
            "\nResult:\n"
            "\"hash\"         (string) The block hash\n"
            "\nExamples:\n" +
            HelpExampleCli("getblockhashexplorer", "1000") + HelpExampleRpc("getblockhashexplorer", "1000"));

    int64_t nHeight = (int64_t)(params[0].get_int());
    if (nHeight < 0 || nHeight > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

    const CBlockIndex* block_index = getexplorerBlockIndex2(nHeight);

    if(block_index == nullptr)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block not found");


    result = BlockToString2((CBlockIndex*)block_index);


    return result;
}

void getNextIn2(const COutPoint& Out, uint256& Hash, unsigned int& n)
{
    // Hash = 0;
    // n = 0;
    // if (paddressmap)
    //    paddressmap->ReadNextIn(Out, Hash, n);
}

UniValue TxToString2(uint256 BlockHash, const CTransaction& tx)
{
    CAmount Input = 0;
    CAmount Output = tx.GetValueOut();
    UniValue info(UniValue::VOBJ);
    UniValue in_txes(UniValue::VOBJ);
    UniValue out_txes(UniValue::VOBJ);

    std::string InputsContentCells[] = {"#", "Taken from", "Address", "Amount"};
    std::string InputsContent = makeHTMLTableRow(InputsContentCells, sizeof(InputsContentCells) / sizeof(std::string));
    std::string OutputsContentCells[] = {"#", "Redeemed in", "Address", "Amount"};
    std::string OutputsContent = makeHTMLTableRow(OutputsContentCells, sizeof(OutputsContentCells) / sizeof(std::string));

    if (tx.IsCoinBase()) {
        std::string InputsContentCells[] =
            {
                "0",
                "coinbase",
                "-",
                ValueToString(Output)};
        InputsContent += makeHTMLTableRow(InputsContentCells, sizeof(InputsContentCells) / sizeof(std::string));

        UniValue tx(UniValue::VOBJ);
        tx.push_back(Pair("from","coinbase"));
        tx.push_back(Pair("number",0));
        tx.push_back(Pair("address","-"));
        tx.push_back(Pair("amount",ValueToString2(Output)));
        in_txes.push_back(Pair("0",tx));

    } else
        for (unsigned int i = 0; i < tx.vin.size(); i++) {
            COutPoint Out = tx.vin[i].prevout;
            CTxOut PrevOut = getPrevOut2(tx.vin[i].prevout);
            if (PrevOut.nValue < 0)
                Input = -Params().MaxMoneyOut();
            else
                Input += PrevOut.nValue;
            std::string InputsContentCells[] =
                {
                    itostr(i),
                    "<span>" + makeHRef(Out.hash.GetHex()) + ":" + itostr(Out.n) + "</span>",
                    ScriptToString(PrevOut.scriptPubKey, true),
                    ValueToString(PrevOut.nValue)};
            InputsContent += makeHTMLTableRow(InputsContentCells, sizeof(InputsContentCells) / sizeof(std::string));

            UniValue tx(UniValue::VOBJ);
            tx.push_back(Pair("from",Out.hash.GetHex()));
            tx.push_back(Pair("number",itostr(Out.n)));
            tx.push_back(Pair("address",ScriptToString2(PrevOut.scriptPubKey, true)));
            tx.push_back(Pair("amount",ValueToString2(PrevOut.nValue)));
            in_txes.push_back(Pair(itostr(i),tx));
        }

    uint256 TxHash = tx.GetHash();
    for (unsigned int i = 0; i < tx.vout.size(); i++) {
        const CTxOut& Out = tx.vout[i];
        uint256 HashNext = uint256S("0");
        unsigned int nNext = 0;
        bool fAddrIndex = false;
        getNextIn2(COutPoint(TxHash, i), HashNext, nNext);
        std::string OutputsContentCells[] =
            {
                itostr(i),
                (HashNext == uint256S("0")) ? (fAddrIndex ? "no" : "unknown") : "<span>" + makeHRef(HashNext.GetHex()) + ":" + itostr(nNext) + "</span>",
                ScriptToString(Out.scriptPubKey, true),
                ValueToString(Out.nValue)};
        OutputsContent += makeHTMLTableRow(OutputsContentCells, sizeof(OutputsContentCells) / sizeof(std::string));

        UniValue tx(UniValue::VOBJ);
        tx.push_back(Pair("redeemed_in",(HashNext == uint256S("0")) ? (fAddrIndex ? "no" : "unknown") : makeHRef(HashNext.GetHex()) + ":" + itostr(nNext) ));
        tx.push_back(Pair("address",ScriptToString2(Out.scriptPubKey, true)));
        tx.push_back(Pair("amount",ValueToString2(Out.nValue)));
        out_txes.push_back(Pair(itostr(i),tx));
    }

    InputsContent = table + InputsContent + "</table>";
    OutputsContent = table + OutputsContent + "</table>";

    std::string Hash = TxHash.GetHex();

    std::string Labels[] =
        {
            "In Block", "",
            "Size", itostr(GetSerializeSize(tx, SER_NETWORK, PROTOCOL_VERSION)),
            "Input", tx.IsCoinBase() ? "-" : ValueToString(Input),
            "Output", ValueToString(Output),
            "Fees", tx.IsCoinBase() ? "-" : ValueToString(Input - Output),
            "Timestamp", "",
            "Hash", "<pre>" + Hash + "</pre>",
        };

    info.push_back(Pair("size",itostr(GetSerializeSize(tx, SER_NETWORK, PROTOCOL_VERSION))));
    info.push_back(Pair("input",tx.IsCoinBase() ? "-" : ValueToString2(Input)));
    info.push_back(Pair("output",ValueToString2(Output)));
    info.push_back(Pair("fees",tx.IsCoinBase() ? "-" : ValueToString2(Input - Output)));
    info.push_back(Pair("hash",Hash));

    info.push_back(Pair("out_txes",out_txes));
    info.push_back(Pair("in_txes",in_txes));


    // std::map<uint256, CBlockIndex*>::iterator iter = mapBlockIndex.find(BlockHash);
    BlockMap::iterator iter = mapBlockIndex.find(BlockHash);
    bool height_time_parsed = false;
    if (iter != mapBlockIndex.end()) {
        CBlockIndex* pIndex = iter->second;
       // Labels[0 * 2 + 1] = makeHRef(itostr(pIndex->nHeight));
       // Labels[5 * 2 + 1] = TimeToString(pIndex->nTime);
        height_time_parsed = true;
        info.pushKV("height",itostr(pIndex->nHeight));
        info.pushKV("timestamp",TimeToString(pIndex->nTime));
    }

    if(!height_time_parsed){
        info.pushKV("height","");
        info.pushKV("timestamp","");
    }
//    std::string Content;
//    Content += "<h2>Transaction&nbsp;<span>" + Hash + "</span></h2>";
//    Content += makeHTMLTable(Labels, sizeof(Labels) / (2 * sizeof(std::string)), 2);
//    Content += "</br>";
//    Content += "<h3>Inputs</h3>";
//    Content += InputsContent;
//    Content += "</br>";
//    Content += "<h3>Outputs</h3>";
//    Content += OutputsContent;

    return info;
}


UniValue gettxexplorer(const UniValue& params, bool fHelp)
{
    UniValue result(UniValue::VOBJ);

    if (fHelp || params.size() != 1)
        throw runtime_error(
            "gettxexplorer txid\n"
            "\nReturns tx of txid in best-block-chain.\n"
            "\nArguments:\n"
            "1. txid         (string, required) The txid\n"
            "\nResult:\n"
            "\"hash\"         (string) The block txid\n"
            "\nExamples:\n" +
            HelpExampleCli("gettxexplorer", "dae45bd9250b18a940cde56c92ac9821d868cb386ee5a18fbb85885a911438c5") + HelpExampleRpc("gettxexplorer", "dae45bd9250b18a940cde56c92ac9821d868cb386ee5a18fbb85885a911438c5"));

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);

    CTransaction tx;
    uint256 hashBlock = 0;
   // std::string tx_str;
    if (GetTransaction(hash, tx, hashBlock, true)) {
        result = TxToString2(hashBlock, tx);
    }
    else
    {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Tx not found");
    }

//    result.push_back(Pair("txid", strHash));
//    result.push_back(Pair("tx_str", tx_str));

    return result;
}


std::string AddressToString2(const CBitcoinAddress& Address)
{
    std::string TxLabels[] =
        {
            "Date",
            "Hash",
            "From",
            "Amount",
            "To",
            "Amount",
            "Delta",
            "Balance"};
    std::string TxContent = table + makeHTMLTableRow(TxLabels, sizeof(TxLabels) / sizeof(std::string));

    std::set<COutPoint> PrevOuts;
    /*
    CScript AddressScript;
    AddressScript.SetDestination(Address.Get());

    CAmount Sum = 0;
    bool fAddrIndex = false;

    if (!fAddrIndex)
        return ""; // it will take too long to find transactions by address
    else
    {
        std::vector<CDiskTxPos> Txs;
        paddressmap->GetTxs(Txs, AddressScript.GetID());
        BOOST_FOREACH (const CDiskTxPos& pos, Txs)
        {
            CTransaction tx;
            CBlock block;
            uint256 bhash = block.GetHash();
            GetTransaction(pos.nTxOffset, tx, bhash);
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(block.GetHash());
            if (mi == mapBlockIndex.end())
                continue;
            CBlockIndex* pindex = (*mi).second;
            if (!pindex || !chainActive.Contains(pindex))
                continue;
            std::string Prepend = "<a href=\"" + itostr(pindex->nHeight) + "\">" + TimeToString(pindex->nTime) + "</a>";
            TxContent += TxToRow(tx, AddressScript, Prepend, &Sum);
        }
    }
    */
    TxContent += "</table>";

    std::string Content;
    Content += "<h1>Transactions to/from&nbsp;<span>" + Address.ToString() + "</span></h1>";
    Content += TxContent;
    return Content;
}

UniValue getblocksinfoexplorer(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() != 2)
        throw runtime_error(
            "getblocksinfoexplorer from to\n"
            "\nReturns blocksinfo from height \"from\" to height \"to\" in best-block-chain.\n"
            "\nArguments:\n"
            "1. from         (numeric, required) The height from\n"
            "2. to         (numeric, required) The height to\n"
            "\nResult:\n"
            "\"hash\"         (string) The height from\n"
            "\nExamples:\n" +
            HelpExampleCli("getblocksinfoexplorer", "10 25") + HelpExampleRpc("getblocksinfoexplorer","10 25"));

    int64_t from = (int64_t)(params[0].get_int());



    int64_t to = (int64_t)(params[1].get_int());


    if (from < 0 || from > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block from out of range");

    if (to < 0 || to > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block to out of range");

    if (to < from)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block to out of range");

    return BlocksToString(from,to);

}


UniValue getqueryexplorer(const UniValue& params, bool fHelp)
{
    UniValue result(UniValue::VOBJ);

    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getqueryexplorer hash\n"
            "\nReturns info of hash in best-block-chain.\n"
            "\nArguments:\n"
            "1. hash         (string, required) The hash\n"
            "\nResult:\n"
            "\"hash\"         (string) The hash\n"
            "\nExamples:\n" +
            HelpExampleCli("getqueryexplorer", "UWTD1rHBFXG5dtvNDuabnXSFfh7W8XhUeD") + HelpExampleRpc("getqueryexplorer", "UWTD1rHBFXG5dtvNDuabnXSFfh7W8XhUeD"));

    std::string strHash;

    try
    {
        strHash = params[0].get_str();
    }
    catch(std::runtime_error& e)
    {
        int64_t nHeight = (int64_t)(params[0].get_int());
        if (nHeight < 0 || nHeight > chainActive.Height())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

        const CBlockIndex* block_index = getexplorerBlockIndex2(nHeight);
        if(block_index == nullptr)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Block not found");

        result = BlockToString2((CBlockIndex*)block_index);
        result.push_back(Pair("type","block"));
        return result;

    }

    // If the query is not an integer, assume it is a block hash
    uint256 hash = uint256S(strHash);

    // std::map<uint256, CBlockIndex*>::iterator iter = mapBlockIndex.find(hash);
    BlockMap::iterator iter = mapBlockIndex.find(hash);
    if (iter != mapBlockIndex.end()) {
        result = BlockToString2(iter->second);
        result.push_back(Pair("type","block"));
        return result;
    }

    // If the query is neither an integer nor a block hash, assume a transaction hash
    CTransaction tx;
    uint256 hashBlock = 0;
   // std::string tx_str;
    if (GetTransaction(hash, tx, hashBlock, true)) {
        result = TxToString2(hashBlock, tx);
        result.push_back(Pair("type","transaction"));
//        result.push_back(Pair("tx_str", tx_str));
        return result;
    }


    // If the query is not an integer, nor a block hash, nor a transaction hash, assume an address
    CBitcoinAddress Address;
    std::string address_str;

    Address.SetString(strHash);
    if (Address.IsValid()) {
        address_str = AddressToString2(Address);
        if (!address_str.empty())
        {
            result.push_back(Pair("type","address"));

            result.push_back(Pair("hash",strHash));
            result.push_back(Pair("address_str", address_str));
            return result;
        }
    }

    throw JSONRPCError(RPC_INVALID_PARAMETER, "Query Invalid");


    return false;

}

UniValue getaddressexplorer(const UniValue& params, bool fHelp)
{
    UniValue result(UniValue::VOBJ);

    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddressexplorer address\n"
            "\nReturns txes of address in best-block-chain.\n"
            "\nArguments:\n"
            "1. txid         (string, required) The address\n"
            "\nResult:\n"
            "\"hash\"         (string) The address\n"
            "\nExamples:\n" +
            HelpExampleCli("getaddressexplorer", "UWTD1rHBFXG5dtvNDuabnXSFfh7W8XhUeD") + HelpExampleRpc("getaddressexplorer", "UWTD1rHBFXG5dtvNDuabnXSFfh7W8XhUeD"));

    std::string strHash = params[0].get_str();
    std::string address_str;
    CBitcoinAddress Address;

    Address.SetString(strHash);
    if (Address.IsValid()) {
        address_str = AddressToString2(Address);
        if (address_str.empty())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Address No Txes");
    }
    else
    {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Address Invalid");
    }

    result.push_back(Pair("address", strHash));
    result.push_back(Pair("address_str", address_str));


    return result;
}


UniValue getblock(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getblock \"hash\" ( verbose )\n"
            "\nIf verbose is false, returns a string that is serialized, hex-encoded data for block 'hash'.\n"
            "If verbose is true, returns an Object with information about block <hash>.\n"
            "\nArguments:\n"
            "1. \"hash\"          (string, required) The block hash\n"
            "2. verbose           (boolean, optional, default=true) true for a json object, false for the hex encoded data\n"
            "\nResult (for verbose = true):\n"
            "{\n"
            "  \"hash\" : \"hash\",     (string) the block hash (same as provided)\n"
            "  \"confirmations\" : n,   (numeric) The number of confirmations, or -1 if the block is not on the main chain\n"
            "  \"size\" : n,            (numeric) The block size\n"
            "  \"height\" : n,          (numeric) The block height or index\n"
            "  \"version\" : n,         (numeric) The block version\n"
            "  \"merkleroot\" : \"xxxx\", (string) The merkle root\n"
            "  \"tx\" : [               (array of string) The transaction ids\n"
            "     \"transactionid\"     (string) The transaction id\n"
            "     ,...\n"
            "  ],\n"
            "  \"time\" : ttt,          (numeric) The block time in seconds since epoch (Jan 1 1970 GMT)\n"
            "  \"nonce\" : n,           (numeric) The nonce\n"
            "  \"bits\" : \"1d00ffff\", (string) The bits\n"
            "  \"difficulty\" : x.xxx,  (numeric) The difficulty\n"
            "  \"previousblockhash\" : \"hash\",  (string) The hash of the previous block\n"
            "  \"nextblockhash\" : \"hash\"       (string) The hash of the next block\n"
            "  \"moneysupply\" : \"supply\"       (numeric) The money supply when this block was added to the blockchain\n"
            "  \"zULOsupply\" :\n"
            "  {\n"
            "     \"1\" : n,            (numeric) supply of 1 zULO denomination\n"
            "     \"5\" : n,            (numeric) supply of 5 zULO denomination\n"
            "     \"10\" : n,           (numeric) supply of 10 zULO denomination\n"
            "     \"50\" : n,           (numeric) supply of 50 zULO denomination\n"
            "     \"100\" : n,          (numeric) supply of 100 zULO denomination\n"
            "     \"500\" : n,          (numeric) supply of 500 zULO denomination\n"
            "     \"1000\" : n,         (numeric) supply of 1000 zULO denomination\n"
            "     \"5000\" : n,         (numeric) supply of 5000 zULO denomination\n"
            "     \"total\" : n,        (numeric) The total supply of all zULO denominations\n"
            "  }\n"
            "}\n"
            "\nResult (for verbose=false):\n"
            "\"data\"             (string) A string that is serialized, hex-encoded data for block 'hash'.\n"
            "\nExamples:\n" +
            HelpExampleCli("getblock", "\"00000000000fd08c2fb661d2fcb0d49abb3a91e5f27082ce64feed3b4dede2e2\"") + HelpExampleRpc("getblock", "\"00000000000fd08c2fb661d2fcb0d49abb3a91e5f27082ce64feed3b4dede2e2\""));

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);

    bool fVerbose = true;
    if (params.size() > 1)
        fVerbose = params[1].get_bool();

    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];

    if (!ReadBlockFromDisk(block, pblockindex))
        throw JSONRPCError(RPC_INTERNAL_ERROR, "Can't read block from disk");

    if (!fVerbose) {
        CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
        ssBlock << block;
        std::string strHex = HexStr(ssBlock.begin(), ssBlock.end());
        return strHex;
    }

    return blockToJSON(block, pblockindex);
}

UniValue getblockheader(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getblockheader \"hash\" ( verbose )\n"
            "\nIf verbose is false, returns a string that is serialized, hex-encoded data for block 'hash' header.\n"
            "If verbose is true, returns an Object with information about block <hash> header.\n"
            "\nArguments:\n"
            "1. \"hash\"          (string, required) The block hash\n"
            "2. verbose           (boolean, optional, default=true) true for a json object, false for the hex encoded data\n"
            "\nResult (for verbose = true):\n"
            "{\n"
            "  \"version\" : n,         (numeric) The block version\n"
            "  \"previousblockhash\" : \"hash\",  (string) The hash of the previous block\n"
            "  \"merkleroot\" : \"xxxx\", (string) The merkle root\n"
            "  \"time\" : ttt,          (numeric) The block time in seconds since epoch (Jan 1 1970 GMT)\n"
            "  \"bits\" : \"1d00ffff\", (string) The bits\n"
            "  \"nonce\" : n,           (numeric) The nonce\n"
            "}\n"
            "\nResult (for verbose=false):\n"
            "\"data\"             (string) A string that is serialized, hex-encoded data for block 'hash' header.\n"
            "\nExamples:\n" +
            HelpExampleCli("getblockheader", "\"00000000000fd08c2fb661d2fcb0d49abb3a91e5f27082ce64feed3b4dede2e2\"") + HelpExampleRpc("getblockheader", "\"00000000000fd08c2fb661d2fcb0d49abb3a91e5f27082ce64feed3b4dede2e2\""));

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);

    bool fVerbose = true;
    if (params.size() > 1)
        fVerbose = params[1].get_bool();

    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];

    if (!ReadBlockFromDisk(block, pblockindex))
        throw JSONRPCError(RPC_INTERNAL_ERROR, "Can't read block from disk");

    if (!fVerbose) {
        CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
        ssBlock << block.GetBlockHeader();
        std::string strHex = HexStr(ssBlock.begin(), ssBlock.end());
        return strHex;
    }

    return blockHeaderToJSON(block, pblockindex);
}

UniValue gettxoutsetinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "gettxoutsetinfo\n"
            "\nReturns statistics about the unspent transaction output set.\n"
            "Note this call may take some time.\n"
            "\nResult:\n"
            "{\n"
            "  \"height\":n,     (numeric) The current block height (index)\n"
            "  \"bestblock\": \"hex\",   (string) the best block hash hex\n"
            "  \"transactions\": n,      (numeric) The number of transactions\n"
            "  \"txouts\": n,            (numeric) The number of output transactions\n"
            "  \"bytes_serialized\": n,  (numeric) The serialized size\n"
            "  \"hash_serialized\": \"hash\",   (string) The serialized hash\n"
            "  \"total_amount\": x.xxx          (numeric) The total amount\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("gettxoutsetinfo", "") + HelpExampleRpc("gettxoutsetinfo", ""));

    UniValue ret(UniValue::VOBJ);

    CCoinsStats stats;
    FlushStateToDisk();
    if (pcoinsTip->GetStats(stats)) {
        ret.push_back(Pair("height", (int64_t)stats.nHeight));
        ret.push_back(Pair("bestblock", stats.hashBlock.GetHex()));
        ret.push_back(Pair("transactions", (int64_t)stats.nTransactions));
        ret.push_back(Pair("txouts", (int64_t)stats.nTransactionOutputs));
        ret.push_back(Pair("bytes_serialized", (int64_t)stats.nSerializedSize));
        ret.push_back(Pair("hash_serialized", stats.hashSerialized.GetHex()));
        ret.push_back(Pair("total_amount", ValueFromAmount(stats.nTotalAmount)));
    }
    return ret;
}

UniValue gettxout(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3)
        throw runtime_error(
            "gettxout \"txid\" n ( includemempool )\n"
            "\nReturns details about an unspent transaction output.\n"
            "\nArguments:\n"
            "1. \"txid\"       (string, required) The transaction id\n"
            "2. n              (numeric, required) vout value\n"
            "3. includemempool  (boolean, optional) Whether to included the mem pool\n"
            "\nResult:\n"
            "{\n"
            "  \"bestblock\" : \"hash\",    (string) the block hash\n"
            "  \"confirmations\" : n,       (numeric) The number of confirmations\n"
            "  \"value\" : x.xxx,           (numeric) The transaction value in btc\n"
            "  \"scriptPubKey\" : {         (json object)\n"
            "     \"asm\" : \"code\",       (string) \n"
            "     \"hex\" : \"hex\",        (string) \n"
            "     \"reqSigs\" : n,          (numeric) Number of required signatures\n"
            "     \"type\" : \"pubkeyhash\", (string) The type, eg pubkeyhash\n"
            "     \"addresses\" : [          (array of string) array of eulo addresses\n"
            "     \"euloaddress\"   	 	(string) eulo address\n"
            "        ,...\n"
            "     ]\n"
            "  },\n"
            "  \"version\" : n,            (numeric) The version\n"
            "  \"coinbase\" : true|false   (boolean) Coinbase or not\n"
            "}\n"

            "\nExamples:\n"
            "\nGet unspent transactions\n" +
            HelpExampleCli("listunspent", "") +
            "\nView the details\n" + HelpExampleCli("gettxout", "\"txid\" 1") +
            "\nAs a json rpc call\n" + HelpExampleRpc("gettxout", "\"txid\", 1"));

    UniValue ret(UniValue::VOBJ);

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);
    int n = params[1].get_int();
    bool fMempool = true;
    if (params.size() > 2)
        fMempool = params[2].get_bool();

    CCoins coins;
    if (fMempool) {
        LOCK(mempool.cs);
        CCoinsViewMemPool view(pcoinsTip, mempool);
        if (!view.GetCoins(hash, coins))
            return NullUniValue;
        mempool.pruneSpent(hash, coins); // TODO: this should be done by the CCoinsViewMemPool
    } else {
        if (!pcoinsTip->GetCoins(hash, coins))
            return NullUniValue;
    }
    if (n < 0 || (unsigned int)n >= coins.vout.size() || coins.vout[n].IsNull())
        return NullUniValue;

    BlockMap::iterator it = mapBlockIndex.find(pcoinsTip->GetBestBlock());
    CBlockIndex* pindex = it->second;
    ret.push_back(Pair("bestblock", pindex->GetBlockHash().GetHex()));
    if ((unsigned int)coins.nHeight == MEMPOOL_HEIGHT)
        ret.push_back(Pair("confirmations", 0));
    else
        ret.push_back(Pair("confirmations", pindex->nHeight - coins.nHeight + 1));
    ret.push_back(Pair("value", ValueFromAmount(coins.vout[n].nValue)));
    UniValue o(UniValue::VOBJ);
    ScriptPubKeyToJSON(coins.vout[n].scriptPubKey, o, true);
    ret.push_back(Pair("scriptPubKey", o));
    ret.push_back(Pair("version", coins.nVersion));
    ret.push_back(Pair("coinbase", coins.fCoinBase));

    return ret;
}

UniValue verifychain(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 2)
        throw runtime_error(
            "verifychain ( numblocks )\n"
            "\nVerifies blockchain database.\n"
            "\nArguments:\n"
            "1. numblocks    (numeric, optional, default=288, 0=all) The number of blocks to check.\n"
            "\nResult:\n"
            "true|false       (boolean) Verified or not\n"
            "\nExamples:\n" +
            HelpExampleCli("verifychain", "") + HelpExampleRpc("verifychain", ""));

    int nCheckLevel = 4;
    int nCheckDepth = GetArg("-checkblocks", 288);
    if (params.size() > 0)
        nCheckDepth = params[1].get_int();

    return CVerifyDB().VerifyDB(pcoinsTip, nCheckLevel, nCheckDepth);
}

/////////////////////////////////////////////////////eulo-vm
UniValue getaccountinfo(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp || params.size() < 1)
        throw std::runtime_error(
                "getaccountinfo \"address\"\n"
                "\nArgument:\n"
                "1. \"address\"          (string, required) The account address\n");

    LOCK(cs_main);

    std::string strAddr = params[0].get_str();
    if (strAddr.size() != 40 || !IsHex(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Incorrect address");

    if (!AddressInUse(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Address does not exist");

    dev::Address addrAccount(strAddr);

    UniValue result(UniValue::VOBJ);

    result.push_back(Pair("address", strAddr));
    result.push_back(Pair("balance", GetContractBalance(addrAccount)));
    std::vector<uint8_t> code = GetContractCode(addrAccount);

    std::map<dev::h256, std::pair<dev::u256, dev::u256>> storage = GetStorageByAddress(strAddr);

    UniValue storageUV(UniValue::VOBJ);
    for (auto j: storage)
    {
        UniValue e(UniValue::VOBJ);
        e.push_back(Pair(j.second.first.str(0, std::ios_base::hex).c_str(), j.second.second.str(0, std::ios_base::hex).c_str()));
        storageUV.push_back(Pair(j.first.hex(), e));
    }

    result.push_back(Pair("storage", storageUV));

    result.push_back(Pair("code", HexStr(code.begin(), code.end())));

    dev::h256 hash;
    uint32_t nVout;
    dev::u256 value;
    uint8_t alive;
    if (GetContractVin(addrAccount, hash, nVout, value, alive))
    {
        UniValue vin(UniValue::VOBJ);
        valtype vchHash(hash.asBytes());
        vin.push_back(Pair("hash", HexStr(vchHash.rbegin(), vchHash.rend())));
        vin.push_back(Pair("nVout", uint64_t(nVout)));
        vin.push_back(Pair("value", uint64_t(value)));
        vin.push_back(Pair("alive", uint8_t(alive)));
        result.push_back(Pair("vin", vin));
    }

    return result;
}

UniValue getstorage(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp || params.size() < 1)
        throw std::runtime_error(
                "getstorage \"address\"\n"
                "\nArgument:\n"
                "1. \"address\"          (string, required) The address to get the storage from\n"
                "2. \"blockNum\"         (string, optional) Number of block to get state from, \"latest\" keyword supported. Latest if not passed.\n"
                "3. \"index\"            (number, optional) Zero-based index position of the storage\n");

    LOCK(cs_main);

    std::string strAddr = params[0].get_str();
    if (strAddr.size() != 40 || !IsHex(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Incorrect address");

    if (params.size() > 1)
    {
        if (params[1].isNum())
        {
            auto blockNum = params[1].get_int();
            if ((blockNum < 0 && blockNum != -1) || blockNum > chainActive.Height())
                throw JSONRPCError(RPC_INVALID_PARAMS, "Incorrect block number");

            if (blockNum != -1)
            {
                uint256 hashStateRoot;
                uint256 hashUTXORoot;
                CBlock block;
                if (!ReadBlockFromDisk(block, chainActive[blockNum]))
                {
                    std::ostringstream stringStream;
                    stringStream << "ReadBlockFromDisk failed at hegiht " << chainActive[blockNum]->nHeight << " hash: " << chainActive[blockNum]->GetBlockHash().ToString();
                    throw JSONRPCError(RPC_INVALID_PARAMS, stringStream.str());
                } else
                {
                    if(block.GetVMState(hashStateRoot, hashUTXORoot) == RET_VM_STATE_ERR){
                        throw JSONRPCError(RPC_INVALID_PARAMS, "Incorrect GetVMState");
                    }
                }
                SetTemporaryState(hashStateRoot, hashUTXORoot);
                //                ifContractObj->SetTemporaryState(chainActive[blockNum]->hashStateRoot,
                //                                                 chainActive[blockNum]->hashUTXORoot);
            }
        } else
        {
            throw JSONRPCError(RPC_INVALID_PARAMS, "Incorrect block number");
        }
    }

    if (!AddressInUse(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Address does not exist");

    UniValue result(UniValue::VOBJ);

    bool onlyIndex = params.size() > 2;
    unsigned index = 0;
    if (onlyIndex)
        index = params[2].get_int();

    std::map<dev::h256, std::pair<dev::u256, dev::u256>> storage = GetStorageByAddress(strAddr);
    if (onlyIndex)
    {
        if (index >= storage.size())
        {
            std::ostringstream stringStream;
            stringStream << "Storage size: " << storage.size() << " got index: " << index;
            throw JSONRPCError(RPC_INVALID_PARAMS, stringStream.str());
        }
        auto elem = std::next(storage.begin(), index);
        UniValue e(UniValue::VOBJ);

        storage = {{elem->first, {elem->second.first, elem->second.second}}};
    }
    for (const auto &j: storage)
    {
        UniValue e(UniValue::VOBJ);
        e.push_back(Pair(j.second.first.str(0, std::ios_base::hex).c_str(), j.second.second.str(0, std::ios_base::hex).c_str()));
        result.push_back(Pair(j.first.hex(), e));
    }

    return result;
}


UniValue callcontract(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
         throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp || params.size() < 2)
        throw std::runtime_error(
                "callcontract \"address\" \"data\" ( address )\n"
                "\nArgument:\n"
                "1. \"address\"          (string, required) The account address\n"
                "2. \"data\"             (string, required) The data hex string\n"
                "3. address              (string, optional) The sender address hex string\n"
                "4. gasLimit  (numeric or string, optional) gasLimit, default: " +
                i64tostr(DEFAULT_GAS_LIMIT_OP_SEND) + "\n");

    LOCK(cs_main);

    std::string strAddr = params[0].get_str();
    std::string data = params[1].get_str();

    if (!IsHex(data))
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid data (data not hex)");

    if (strAddr.size() != 40 || !IsHex(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Incorrect address");


    CBlockIndex * pBlockIndex = chainActive.Tip();
    if (pBlockIndex->nHeight < Params().Contract_StartHeight())
        throw JSONRPCError(RPC_INVALID_REQUEST, "contract not enabled.");

    if (AddressInUse(strAddr))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Address does not exist");

    string sender = "";
    if (params.size() == 3)
    {
        CBitcoinAddress btcSenderAddress(params[2].get_str());
        if (btcSenderAddress.IsValid())
        {
            CKeyID keyid;
            btcSenderAddress.GetKeyID(keyid);

            sender = HexStr(valtype(keyid.begin(), keyid.end()));
        } else
        {
            sender = params[2].get_str();
        }
    }
    uint64_t gasLimit = 0;
    if (params.size() == 4)
    {
        //        gasLimit = params[3].get_int();
        if (params[3].isNum())
        {
            gasLimit = params[3].get_int64();
        } else if (params[3].isStr())
        {
            gasLimit = atoi64(params[3].get_str());
        } else
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "JSON value for gasLimit is not (numeric or string)");
        }

    }

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("address", strAddr));

    RPCCallContract(result, strAddr, ParseHex(data), sender, gasLimit);

    return result;
}

void assignJSON(UniValue& entry, const TransactionReceiptInfo& resExec) {
    entry.push_back(Pair("blockHash", resExec.blockHash.GetHex()));
    entry.push_back(Pair("blockNumber", uint64_t(resExec.blockNumber)));
    entry.push_back(Pair("transactionHash", resExec.transactionHash.GetHex()));
    entry.push_back(Pair("transactionIndex", uint64_t(resExec.transactionIndex)));
    entry.push_back(Pair("from", resExec.from.hex()));
    entry.push_back(Pair("to", resExec.to.hex()));
    entry.push_back(Pair("cumulativeGasUsed", CAmount(resExec.cumulativeGasUsed)));
    entry.push_back(Pair("gasUsed", CAmount(resExec.gasUsed)));
    entry.push_back(Pair("contractAddress", resExec.contractAddress.hex()));
    std::stringstream ss;
    ss << resExec.excepted;
    entry.push_back(Pair("excepted",ss.str()));
}

void assignJSON(UniValue& logEntry, const dev::eth::LogEntry& log,
        bool includeAddress) {
    if (includeAddress) {
        logEntry.push_back(Pair("address", log.address.hex()));
    }

    UniValue topics(UniValue::VARR);
    for (dev::h256 hash : log.topics) {
        topics.push_back(hash.hex());
    }
    logEntry.push_back(Pair("topics", topics));
    logEntry.push_back(Pair("data", HexStr(log.data)));
}

void transactionReceiptInfoToJSON(const TransactionReceiptInfo& resExec, UniValue& entry) {
    assignJSON(entry, resExec);

    const auto& logs = resExec.logs;
    UniValue logEntries(UniValue::VARR);
    for(const auto&log : logs){
        UniValue logEntry(UniValue::VOBJ);
        assignJSON(logEntry, log, true);
        logEntries.push_back(logEntry);
    }
    entry.push_back(Pair("log", logEntries));
}

UniValue searchlogs(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp || params.size() < 2 || params.size() > 5)
        throw std::runtime_error(
                "searchlogs <fromBlock> <toBlock> (address) (topics)\n"
                "requires -logevents to be enabled"
                "\nArgument:\n"
                "1. \"fromBlock\"        (numeric, required) The number of the earliest block (latest may be given to mean the most recent block).\n"
                "2. \"toBlock\"          (string, required) The number of the latest block (-1 may be given to mean the most recent block).\n"
                "3. \"address\"          (string, optional) An address or a list of addresses to only get logs from particular account(s).\n"
                "4. \"topics\"           (string, optional) An array of values from which at least one must appear in the log entries. The order is important, if you want to leave topics out use null, e.g. [\"null\", \"0x00...\"]. \n"
                "5. \"minconf\"          (uint, optional, default=0) Minimal number of confirmations before a log is returned\n"
                "\nExamples:\n"
                + HelpExampleCli("searchlogs",
                "0 100 '{\"addresses\": [\"12ae42729af478ca92c8c66773a3e32115717be4\"]}' '{\"topics\": [\"null\",\"b436c2bf863ccd7b8f63171201efd4792066b4ce8e543dde9c3e9e9ab98e216c\"]}'")
                + HelpExampleRpc("searchlogs",
                "0 100 {\"addresses\": [\"12ae42729af478ca92c8c66773a3e32115717be4\"]} {\"topics\": [\"null\",\"b436c2bf863ccd7b8f63171201efd4792066b4ce8e543dde9c3e9e9ab98e216c\"]}"));

    if (!fLogEvents)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "Events indexing disabled");

    int curheight = 0;

    LOCK(cs_main);

    size_t  fromBlock = 0;
    size_t  toBlock = 0;
    size_t  minconf = 0;

    std::set<dev::h160> addresses;
    std::vector<boost::optional<dev::h256>> topics;

    if (params[0].isNum() && params[0].get_int() >= 0)
        fromBlock = params[0].get_int();
    else if (params[0].isNull() || (params[0].isStr() && !params[0].get_str().compare("latest")))
        fromBlock = chainActive.Height();
    else
        throw JSONRPCError(RPC_INVALID_PARAMS, "invalid from block number");

    if (params[1].isNum() && params[1].get_int() >= 0)
        toBlock = params[1].get_int();
    else if (params[1].isNull() || (params[1].isStr() && !params[1].get_str().compare("latest")))
        toBlock = chainActive.Height();
    else
        throw JSONRPCError(RPC_INVALID_PARAMS, "invalid to block number");

    if (params.size() > 2 && !params[2]["addresses"].isNull())
    {
        if (params[2]["addresses"].isStr())
        {
            std::string address = params[2]["addresses"].get_str();

            if (address.length() != 40 || !IsHex(address))
                throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string address");

            addresses.insert(dev::h160(address));
        }
        else if (params[2]["addresses"].isArray())
        {
            auto value = params[2]["addresses"].getValues();

            for (size_t index = 0; index < value.size(); index++)
            {            
                std::string address;
                
                if (!value[index].isStr())
                    throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string in address array");
                
                address = value[index].get_str();
                if (address.length() != 40 || !IsHex(address))
                    throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string or size in address array");

                addresses.insert(dev::h160(address));
            }
        }
        else
            throw JSONRPCError(RPC_INVALID_PARAMS, "Expect an string or array of hex 160 strings address");
    }

    if(params.size() > 3 && !params[3]["topics"].isNull())
    {
        if (params[3]["topics"].isStr())
        {
            std::string topic = params[3]["topics"].get_str();

            if (topic.length() != 40 || !IsHex(topic))
                throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string topics");

            topics.push_back(boost::optional<dev::h256>(dev::h256(topic)));
        }
        else if (params[3]["topics"].isArray())
        {
            auto value = params[3]["topics"].getValues();

            for (size_t index = 0; index < value.size(); index++)
            {
                std::string topic;
            
                if (!value[index].isStr())
                    throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string in address array");
            
                topic = value[index].get_str();
                if (topic.length() != 40 || !IsHex(topic))
                    throw JSONRPCError(RPC_INVALID_PARAMS, "Invalid hex 160 string or size in address array");
            
                topics.push_back(boost::optional<dev::h256>(dev::h256(topic)));;
            }
        }
        else
            throw JSONRPCError(RPC_INVALID_PARAMS, "Expect an string or array of hex 160 strings topics");
    }

    if (params.size() > 4)
    {
        if (params[4].isNum() && params[4].get_int() >= 0)
            minconf = params[4].get_int();
        else
            throw JSONRPCError(RPC_INVALID_PARAMS, "Expects unsigned integer of minconf");
    }

    std::vector<std::vector<uint256>> hashesToBlock;
    curheight = pblocktree->ReadHeightIndex(fromBlock, toBlock, minconf,
                                                              hashesToBlock,
                                                              addresses);

    if (curheight == -1)
    {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Incorrect params");
    }

    UniValue result(UniValue::VARR);

    for (const auto &hashesTx : hashesToBlock)
    {
        for (const auto &e : hashesTx)
        {
            std::vector<TransactionReceiptInfo> receipts = GetResult(e);

            for (const auto &receipt : receipts)
            {
                if (receipt.logs.empty())
                {
                    continue;
                }

                if (!topics.empty())
                {
                    for (size_t i = 0; i < topics.size(); i++)
                    {
                        const auto &tc = topics[i];

                        if (!tc)
                        {
                            continue;
                        }

                        for (const auto &log: receipt.logs)
                        {
                            auto filterTopicContent = tc.get();

                            if (i >= log.topics.size())
                            {
                                continue;
                            }

                            if (filterTopicContent == log.topics[i])
                            {
                                goto push;
                            }
                        }
                    }

                    // Skip the log if none of the topics are matched
                    continue;
                }

                push:

                UniValue tri(UniValue::VOBJ);
                transactionReceiptInfoToJSON(receipt, tri);
                result.push_back(tri);
            }
        }
    }

    return result;
}

UniValue gettransactionreceipt(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp || params.size() < 1)
        throw std::runtime_error(
                "gettransactionreceipt \"hash\"\n"
                "requires -logevents to be enabled"
                "\nArgument:\n"
                "1. \"hash\"          (string, required) The transaction hash\n");

    if (!fLogEvents)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "Events indexing disabled");

    LOCK(cs_main);

    std::string hashTemp = params[0].get_str();
    if (hashTemp.size() != 64)
    {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Incorrect hash");
    }

    uint256 hash(uint256S(hashTemp));

    std::vector<TransactionReceiptInfo> transactionReceiptInfo = GetResult(hash);

    UniValue result(UniValue::VARR);
    for (TransactionReceiptInfo &t : transactionReceiptInfo)
    {
        UniValue tri(UniValue::VOBJ);
        transactionReceiptInfoToJSON(t, tri);
        result.push_back(tri);
    }

    return result;
}

UniValue listcontracts(const UniValue& params, bool fHelp)
{
    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);
    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    if (fHelp)
        throw std::runtime_error(
                "listcontracts (start maxDisplay)\n"
                "\nArgument:\n"
                "1. start     (numeric or string, optional) The starting account index, default 1\n"
                "2. maxDisplay       (numeric or string, optional) Max accounts to list, default 20\n");

    LOCK(cs_main);

    int start = 1;
    if (params.size() > 0)
    {
        start = params[0].get_int();
        if (start <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid start, min=1");
    }

    int maxDisplay = 20;
    if (params.size() > 1)
    {
        maxDisplay = params[1].get_int();
        if (maxDisplay <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid maxDisplay");
    }

    UniValue result(UniValue::VOBJ);

    std::unordered_map<dev::h160, dev::u256> map = GetContractList();
    int contractsCount = (int)map.size();

    if (contractsCount > 0 && start > contractsCount)
        throw JSONRPCError(RPC_TYPE_ERROR, "start greater than max index " + itostr(contractsCount));

    int itStartPos = std::min(start - 1, contractsCount);
    int i = 0;

    for (auto it = std::next(map.begin(), itStartPos); it != map.end(); it++)
    {
        CAmount balance = GetContractBalance(it->first);
        result.push_back(Pair(it->first.hex(), ValueFromAmount(balance)));
        i++;
        if (i == maxDisplay)
            break;
    }

    return result;
}

///////////////////////////////////////////////////////

UniValue getblockchaininfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getblockchaininfo\n"
            "Returns an object containing various state info regarding block chain processing.\n"
            "\nResult:\n"
            "{\n"
            "  \"chain\": \"xxxx\",        (string) current network name as defined in BIP70 (main, test, regtest)\n"
            "  \"blocks\": xxxxxx,         (numeric) the current number of blocks processed in the server\n"
            "  \"headers\": xxxxxx,        (numeric) the current number of headers we have validated\n"
            "  \"bestblockhash\": \"...\", (string) the hash of the currently best block\n"
            "  \"difficulty\": xxxxxx,     (numeric) the current difficulty\n"
            "  \"verificationprogress\": xxxx, (numeric) estimate of verification progress [0..1]\n"
            "  \"chainwork\": \"xxxx\"     (string) total amount of work in active chain, in hexadecimal\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("getblockchaininfo", "") + HelpExampleRpc("getblockchaininfo", ""));

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("chain", Params().NetworkIDString()));
    obj.push_back(Pair("blocks", (int)chainActive.Height()));
    obj.push_back(Pair("headers", pindexBestHeader ? pindexBestHeader->nHeight : -1));
    obj.push_back(Pair("bestblockhash", chainActive.Tip()->GetBlockHash().GetHex()));
    obj.push_back(Pair("difficulty", (double)GetDifficulty()));
    obj.push_back(Pair("verificationprogress", Checkpoints::GuessVerificationProgress(chainActive.Tip())));
    obj.push_back(Pair("chainwork", chainActive.Tip()->nChainWork.GetHex()));
    return obj;
}

/** Comparison function for sorting the getchaintips heads.  */
struct CompareBlocksByHeight {
    bool operator()(const CBlockIndex* a, const CBlockIndex* b) const
    {
        /* Make sure that unequal blocks with the same height do not compare
           equal. Use the pointers themselves to make a distinction. */

        if (a->nHeight != b->nHeight)
            return (a->nHeight > b->nHeight);

        return a < b;
    }
};

UniValue getchaintips(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getchaintips\n"
            "Return information about all known tips in the block tree,"
            " including the main chain as well as orphaned branches.\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"height\": xxxx,         (numeric) height of the chain tip\n"
            "    \"hash\": \"xxxx\",         (string) block hash of the tip\n"
            "    \"branchlen\": 0          (numeric) zero for main chain\n"
            "    \"status\": \"active\"      (string) \"active\" for the main chain\n"
            "  },\n"
            "  {\n"
            "    \"height\": xxxx,\n"
            "    \"hash\": \"xxxx\",\n"
            "    \"branchlen\": 1          (numeric) length of branch connecting the tip to the main chain\n"
            "    \"status\": \"xxxx\"        (string) status of the chain (active, valid-fork, valid-headers, headers-only, invalid)\n"
            "  }\n"
            "]\n"
            "Possible values for status:\n"
            "1.  \"invalid\"               This branch contains at least one invalid block\n"
            "2.  \"headers-only\"          Not all blocks for this branch are available, but the headers are valid\n"
            "3.  \"valid-headers\"         All blocks are available for this branch, but they were never fully validated\n"
            "4.  \"valid-fork\"            This branch is not part of the active chain, but is fully validated\n"
            "5.  \"active\"                This is the tip of the active main chain, which is certainly valid\n"
            "\nExamples:\n" +
            HelpExampleCli("getchaintips", "") + HelpExampleRpc("getchaintips", ""));

    /* Build up a list of chain tips.  We start with the list of all
       known blocks, and successively remove blocks that appear as pprev
       of another block.  */
    std::set<const CBlockIndex*, CompareBlocksByHeight> setTips;
    BOOST_FOREACH (const PAIRTYPE(const uint256, CBlockIndex*) & item, mapBlockIndex)
        setTips.insert(item.second);
    BOOST_FOREACH (const PAIRTYPE(const uint256, CBlockIndex*) & item, mapBlockIndex) {
        const CBlockIndex* pprev = item.second->pprev;
        if (pprev)
            setTips.erase(pprev);
    }

    // Always report the currently active tip.
    setTips.insert(chainActive.Tip());

    /* Construct the output array.  */
    UniValue res(UniValue::VARR);
    BOOST_FOREACH (const CBlockIndex* block, setTips) {
        UniValue obj(UniValue::VOBJ);
        obj.push_back(Pair("height", block->nHeight));
        obj.push_back(Pair("hash", block->phashBlock->GetHex()));

        const int branchLen = block->nHeight - chainActive.FindFork(block)->nHeight;
        obj.push_back(Pair("branchlen", branchLen));

        string status;
        if (chainActive.Contains(block)) {
            // This block is part of the currently active chain.
            status = "active";
        } else if (block->nStatus & BLOCK_FAILED_MASK) {
            // This block or one of its ancestors is invalid.
            status = "invalid";
        } else if (block->nChainTx == 0) {
            // This block cannot be connected because full block data for it or one of its parents is missing.
            status = "headers-only";
        } else if (block->IsValid(BLOCK_VALID_SCRIPTS)) {
            // This block is fully validated, but no longer part of the active chain. It was probably the active block once, but was reorganized.
            status = "valid-fork";
        } else if (block->IsValid(BLOCK_VALID_TREE)) {
            // The headers for this block are valid, but it has not been validated. It was probably never part of the most-work chain.
            status = "valid-headers";
        } else {
            // No clue.
            status = "unknown";
        }
        obj.push_back(Pair("status", status));

        res.push_back(obj);
    }

    return res;
}

UniValue getfeeinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
                "getfeeinfo blocks\n"
                        "\nReturns details of transaction fees over the last n blocks.\n"
                        "\nArguments:\n"
                        "1. blocks     (int, required) the number of blocks to get transaction data from\n"
                        "\nResult:\n"
                        "{\n"
                        "  \"txcount\": xxxxx                (numeric) Current tx count\n"
                        "  \"txbytes\": xxxxx                (numeric) Sum of all tx sizes\n"
                        "  \"ttlfee\": xxxxx                 (numeric) Sum of all fees\n"
                        "  \"feeperkb\": xxxxx               (numeric) Average fee per kb over the block range\n"
                        "  \"rec_highpriorityfee_perkb\": xxxxx    (numeric) Recommended fee per kb to use for a high priority tx\n"
                        "}\n"
                        "\nExamples:\n" +
                HelpExampleCli("getfeeinfo", "5") + HelpExampleRpc("getfeeinfo", "5"));


    int nBlocks = params[0].get_int();
    int nBestHeight = chainActive.Height();
    int nStartHeight = nBestHeight - nBlocks;
    if (nBlocks < 0 || nStartHeight <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "invalid start height");

    CAmount nFees = 0;
    int64_t nBytes = 0;
    int64_t nTotal = 0;
    for (int i = nStartHeight; i <= nBestHeight; i++) {
        CBlockIndex* pindex = chainActive[i];
        CBlock block;
        if (!ReadBlockFromDisk(block, pindex))
            throw JSONRPCError(RPC_DATABASE_ERROR, "failed to read block from disk");

        CAmount nValueIn = 0;
        CAmount nValueOut = 0;
        for (const CTransaction& tx : block.vtx) {
            if (tx.IsCoinBase() || tx.IsCoinStake())
                continue;

            for (unsigned int j = 0; j < tx.vin.size(); j++) {
                if (tx.vin[j].scriptSig.IsZerocoinSpend()) {
                    nValueIn += tx.vin[j].nSequence * COIN;
                    continue;
                }

                COutPoint prevout = tx.vin[j].prevout;
                CTransaction txPrev;
                uint256 hashBlock;
                if(!GetTransaction(prevout.hash, txPrev, hashBlock, true))
                    throw JSONRPCError(RPC_DATABASE_ERROR, "failed to read tx from disk");
                nValueIn += txPrev.vout[prevout.n].nValue;
            }

            for (unsigned int j = 0; j < tx.vout.size(); j++) {
                nValueOut += tx.vout[j].nValue;
            }

            nFees += nValueIn - nValueOut;
            nBytes += tx.GetSerializeSize(SER_NETWORK, CLIENT_VERSION);
            nTotal++;
        }

        pindex = chainActive.Next(pindex);
        if (!pindex)
            break;
    }

    UniValue ret(UniValue::VOBJ);
    CFeeRate nFeeRate = CFeeRate(nFees, nBytes);
    ret.push_back(Pair("txcount", (int64_t)nTotal));
    ret.push_back(Pair("txbytes", (int64_t)nBytes));
    ret.push_back(Pair("ttlfee", FormatMoney(nFees)));
    ret.push_back(Pair("feeperkb", FormatMoney(nFeeRate.GetFeePerK())));
    ret.push_back(Pair("rec_highpriorityfee_perkb", FormatMoney(nFeeRate.GetFeePerK() + 1000)));

    return ret;
}

UniValue getmempoolinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getmempoolinfo\n"
            "\nReturns details on the active state of the TX memory pool.\n"
            "\nResult:\n"
            "{\n"
            "  \"size\": xxxxx                (numeric) Current tx count\n"
            "  \"bytes\": xxxxx               (numeric) Sum of all tx sizes\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("getmempoolinfo", "") + HelpExampleRpc("getmempoolinfo", ""));

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("size", (int64_t)mempool.size()));
    ret.push_back(Pair("bytes", (int64_t)mempool.GetTotalTxSize()));

    return ret;
}

#ifdef  POW_IN_POS_PHASE

UniValue tmpblockstatus(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "tmpblockstatus \n"
            "\nPermanently marks a block as invalid, as if it violated a consensus rule.\n"
            "\nArguments:\n"
            "\nResult:\n"
            "\nExamples:\n" +
            HelpExampleCli("tmpblockstatus","") + HelpExampleRpc("tmpblockstatus",""));

    UniValue tmpblocks(UniValue::VARR);

    {
        LOCK(cs_main);
        int64_t tmp_blocks_size = tmpblockmempool.mapTmpBlock.size();

        UniValue size_obj(UniValue::VOBJ);
        size_obj.push_back(Pair("size", tmp_blocks_size));
        tmpblocks.push_back(size_obj);

        std::map<uint256, std::pair<CTmpBlockParams,int64_t>>::iterator iter;

        for(iter=tmpblockmempool.mapTmpBlock.begin(); iter!=tmpblockmempool.mapTmpBlock.end(); iter++)
         {
            UniValue onetmpblock(UniValue::VOBJ);

            CTmpBlockParams &tbp = iter->second.first;
            onetmpblock.push_back(Pair("time", (int64_t)(iter->second.second)));
            onetmpblock.push_back(Pair("ori_hash", tbp.ori_hash.GetHex()));
            onetmpblock.push_back(Pair("nNonce", (int64_t)(tbp.nNonce)));
            onetmpblock.push_back(Pair("coinBaseTxhash", (tbp.coinBaseTx.GetHash().GetHex())));
            onetmpblock.push_back(Pair("coinBaseTxout", (int64_t)(tbp.coinBaseTx.GetValueOut())));
            onetmpblock.push_back(Pair("blockheader_hash", tbp.blockheader_hash.GetHex()));

            tmpblocks.push_back(onetmpblock);

        }
    }

    return tmpblocks;
}

#endif

UniValue invalidateblock(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "invalidateblock \"hash\"\n"
            "\nPermanently marks a block as invalid, as if it violated a consensus rule.\n"
            "\nArguments:\n"
            "1. hash   (string, required) the hash of the block to mark as invalid\n"
            "\nResult:\n"
            "\nExamples:\n" +
            HelpExampleCli("invalidateblock", "\"blockhash\"") + HelpExampleRpc("invalidateblock", "\"blockhash\""));

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);
    CValidationState state;

    {
        LOCK(cs_main);
        if (mapBlockIndex.count(hash) == 0)
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

        CBlockIndex* pblockindex = mapBlockIndex[hash];
        InvalidateBlock(state, pblockindex);
    }

    if (state.IsValid()) {
        ActivateBestChain(state);
    }

    if (!state.IsValid()) {
        throw JSONRPCError(RPC_DATABASE_ERROR, state.GetRejectReason());
    }

    return NullUniValue;
}

UniValue reconsiderblock(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "reconsiderblock \"hash\"\n"
            "\nRemoves invalidity status of a block and its descendants, reconsider them for activation.\n"
            "This can be used to undo the effects of invalidateblock.\n"
            "\nArguments:\n"
            "1. hash   (string, required) the hash of the block to reconsider\n"
            "\nResult:\n"
            "\nExamples:\n" +
            HelpExampleCli("reconsiderblock", "\"blockhash\"") + HelpExampleRpc("reconsiderblock", "\"blockhash\""));

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);
    CValidationState state;

    {
        LOCK(cs_main);
        if (mapBlockIndex.count(hash) == 0)
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

        CBlockIndex* pblockindex = mapBlockIndex[hash];
        ReconsiderBlock(state, pblockindex);
    }

    if (state.IsValid()) {
        ActivateBestChain(state);
    }

    if (!state.IsValid()) {
        throw JSONRPCError(RPC_DATABASE_ERROR, state.GetRejectReason());
    }

    return NullUniValue;
}

UniValue getinvalid (const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
                "getinvalid \n"
                        "\nGet a summary of invalidated outpoints.\n"
                        "\nArguments:\n"
                        "1. all   (string, optional) return a full list of outpoints even if they are spent\n"
                        "\nExamples:\n" +
                HelpExampleCli("getinvalid", "\"all\"") + HelpExampleRpc("getinvalid", "\"all\""));

    string strCommand;
    if (params.size() == 1){
        strCommand = params[0].get_str();
    }

    if (strCommand == "serials") {
        UniValue ret(UniValue::VARR);
        CAmount nSerialTotal = 0;
        for (auto it : mapInvalidSerials) {
            UniValue objSerial(UniValue::VOBJ);
            objSerial.push_back(Pair(it.first.GetHex(), FormatMoney(it.second)));
            nSerialTotal += it.second;
            ret.push_back(objSerial);
        }

        UniValue objTotal(UniValue::VOBJ);
        objTotal.push_back(Pair("total_value", FormatMoney(nSerialTotal)));
        ret.push_back(objTotal);
        return ret;
    }

    bool fShowAll = false;
    if (strCommand == "all")
        fShowAll = true;

    CAmount nUnspent = 0;
    CAmount nMint = 0;
    CAmount nMixedValid = 0;
    map<CBitcoinAddress, CAmount> mapBanAddress;
    map<COutPoint, int> mapMixedValid;

    UniValue ret(UniValue::VARR);
    for (auto it : mapInvalidOutPoints) {
        COutPoint out = it.first;
        //Get the tx that the outpoint is from
        CTransaction tx;
        uint256 hashBlock;
        if (!GetTransaction(out.hash, tx, hashBlock, true)) {
            continue;
        }

        UniValue objTx(UniValue::VOBJ);
        objTx.push_back(Pair("inv_out", it.first.ToString()));

        CAmount nValue = tx.vout[out.n].nValue;
        objTx.push_back(Pair("value", FormatMoney(nValue)));

        //Search the txin's to see if any of them are "valid".
        UniValue objMixedValid(UniValue::VOBJ);

        //if some of the other inputs are valid
        for(CTxIn in2 : tx.vin) {
            //See if this is already accounted for
            if(mapInvalidOutPoints.count(in2.prevout) || mapMixedValid.count(in2.prevout))
                continue;

            CTransaction txPrev;
            uint256 hashBlock;
            if(!GetTransaction(in2.prevout.hash, txPrev, hashBlock, true))
                continue;

            //This is a valid outpoint that mixed with an invalid outpoint. Investigate this person.
            //Information leakage, not covering their tracks well enough
            CAmount nValid = txPrev.vout[in2.prevout.n].nValue;
            objMixedValid.push_back(Pair(FormatMoney(nValid), in2.prevout.ToString()));

            nMixedValid += nValid;
            mapMixedValid[in2.prevout] = 1;
        }

        //Check whether this bad outpoint has been spent
        bool fSpent = false;
        CCoinsViewCache cache(pcoinsTip);
        const CCoins* coins = cache.AccessCoins(out.hash);
        if (!coins || !coins->IsAvailable(out.n))
            fSpent = true;

        objTx.push_back(Pair("spent", fSpent));
        if (!objMixedValid.empty())
            objTx.push_back(Pair("mixed_with_valid", objMixedValid));

        CScript scriptPubKey = tx.vout[out.n].scriptPubKey;
        if (scriptPubKey.IsZerocoinMint()) {
            nMint += nValue;
        } else if (!fSpent) {
            CTxDestination dest;
            if (!ExtractDestination(scriptPubKey, dest)) {
                continue;
            }
            CBitcoinAddress address(dest);
            mapBanAddress[address] += nValue;
            nUnspent += nValue;
        }

        if (fSpent && !fShowAll)
            continue;

        ret.push_back(objTx);
    }

    UniValue objAddresses(UniValue::VOBJ);
    for (auto it : mapBanAddress)
        objAddresses.push_back(Pair(it.first.ToString(), FormatMoney(it.second)));

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("addresses_with_invalid", objAddresses));
    obj.push_back(Pair("total_unspent", FormatMoney(nUnspent)));
    obj.push_back(Pair("total_minted", FormatMoney(nMint)));
    obj.push_back(Pair("total_valid_used", FormatMoney(nMixedValid)));

    ret.push_back(obj);
    return ret;
}

UniValue findserial(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() != 1)
        throw runtime_error(
            "findserial \"serial\"\n"
                "\nSearches the zerocoin database for a zerocoin spend transaction that contains the specified serial\n"
                "\nArguments:\n"
                "1. serial   (string, required) the serial of a zerocoin spend to search for.\n"
                "\nResult:\n"
                "{\n"
                "  \"success\": true/false        (boolean) Whether the serial was found\n"
                "  \"txid\": xxxxx                (numeric) The transaction that contains the spent serial\n"
                "}\n"
                "\nExamples:\n" +
            HelpExampleCli("findserial", "\"serial\"") + HelpExampleRpc("findserial", "\"serial\""));

    std::string strSerial = params[0].get_str();
    CBigNum bnSerial = 0;
    bnSerial.SetHex(strSerial);
    if (!bnSerial)
    throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid serial");

    uint256 txid = 0;
    bool fSuccess = zerocoinDB->ReadCoinSpend(bnSerial, txid);

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("success", fSuccess));
    ret.push_back(Pair("txid", txid.GetHex()));

    return ret;
}
