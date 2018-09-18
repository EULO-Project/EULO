// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1510948627, // * UNIX timestamp of last checkpoint block
    0,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1740710,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1454124731,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);

    return &ZCParamsHex;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0xec;
        pchMessageStart[2] = 0xf5;
        pchMessageStart[3] = 0xe5;
        vAlertPubKey = ParseHex("0490e0480bf864eece4ddca8787bb1a74f823361e7e9e931e67385b6976600a98637e2f2eb32568035266789c44e0471443bbe27efd6673284e53d4f16272566f9");
        nDefaultPort = 58802;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // EULO starting difficulty is 1 / 2^20
        nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // EULO: 1 minute
        nTargetSpacing = 1 * 60;  // EULO: 1 minute
        nMaturity = 100;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 21148191990 * COIN;

        /** Height or Time Based Activations **/
        nLastPOWBlock = 1439;
        nPOWStartBlockInPOS = 259200;
        nModifierUpdateBlock = 0;
        nZerocoinStartHeight = 43200;
        nZerocoinStartTime = 1533859200;    // August 10, 2018 08:00:00 AM

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000013ea705a, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4df5d8, nTime=1533859200, nBits=1e00ffff, nNonce=1742924, vtx=1)
         *   CTransaction(hash=4df5d8, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001e01044c5b552e532e204e657773202620576f726c64205265706f7274204175672031302032303138205553206d696772616e74733a204a75646765206f7264657273206465706f72746174696f6e20706c616e65207475726e61726f756e64)
         *     CTxOut(nValue=250.00000000, scriptPubKey=0xC9D02E3B070AEAAB4B7742)
         *   vMerkleTree: 4df5d8
         */
        const char* pszTimestamp = "U.S. News & World Report Aug 10 2018 US migrants: Judge orders deportation plane turnaround";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 503382015 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 250 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04c10e83b2703ccf322f7dbd62dd5855ac7c10bd055814ce121ba32607d573b8810c02c0582aed05b4deb9c4b77b26d92428c61256cd42774babea0a073b2ed0c9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1533859200;
        genesis.nBits = 0x1e00ffff;
        genesis.nNonce = 1742924;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000013ea705af4ed8b40a86250bc6d78b982c604eef80a908cec7f11abbdd1"));
        assert(genesis.hashMerkleRoot == uint256("0x4df5d8d4db708df49c4f911a0946991d5a7628e2e93dfe22b76f9233a9492426"));

        vSeeds.push_back(CDNSSeedData("47.91.79.145", "47.91.79.145"));             // Germany node address
        vSeeds.push_back(CDNSSeedData("47.74.14.246", "47.74.14.246"));             // Japan node address
        vSeeds.push_back(CDNSSeedData("47.74.147.210", "47.74.147.210"));           // Singapore node address
        vSeeds.push_back(CDNSSeedData("47.90.215.200", "47.90.215.200"));           // US node address
        vSeeds.push_back(CDNSSeedData("47.89.243.161", "47.89.243.161"));           // Single node address

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 68);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 13);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 212);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "04f868526d10d4a734e155853f76b4a8be21b941a6891dfbd0f0a5889adf7fcb052d843acddd2af776f6834b71e6c53778eabcecc357a9578ae70d276257fd8b80";
        strObfuscationPoolDummyAddress = "UXJdBngPh9maq59nbyazqptD8qacGJb7ak";
        nStartMasternodePayments = 1514768399; //Wed, 25 Jun 2018 8:59:59 GMT

        /** Zerocoin */
        zerocoinModulus = "b024c540851a3461c17a8da2b39752f5b6f59cd753c0de054a8b2f8b19c2729ef5c5fc99ebc4507d7926934fac50219ff55639316c1e278f4972188ea8bc3bae"
            "fcc0b4f747023b52cbbc8ff71a2e065a7ddd40b86b79753b9f8fd707abed76598c1bbcfa98f14be0655cfd68049f56a6579f0f839d3c0a8a4bc09fb2b70eb310"
            "65512b9935cc1bb76d1ac05d4c61517606ca4c7ddc05d17ec5aa8b7551c5f1c4bca7e6e29ee79515eab5b13d49ba94acb71eab013ca27d346d04e7f814b44b71"
            "0bad17e2551fd48b57260cc481e5e1be765bcdb0e62dc8c76742e5807e6fff8258ece6e84c3b1da8a26ddd1e72c40e6cee46d72d97435cfc8c1fb4feaa67d847"
            "2f56fece6f78dba68ea00e9d1c5c48640f22e2c73511a47f91fd77d11c5f1b411e26e4b25696192ba44bbf567cb43b60b5d312087fe76ad6f9cacd66a58223cf"
            "1f1340f559335a2d0dcca3c8021c3351";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xcf;
        pchMessageStart[1] = 0xcc;
        pchMessageStart[2] = 0xd5;
        pchMessageStart[3] = 0xc5;
        vAlertPubKey = ParseHex("04146d06bd90bf85bec61fb050e0016dc6fd9b078f86ae6f8a2281a861065d40de1cf02788c0a2324e1020cd13bc323d8bdb66e1bf24d241cf601d347f1aa3ad78");
        nDefaultPort = 58804;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // EULO: 1 day
        nTargetSpacing = 1 * 60;  // EULO: 1 minute
        nLastPOWBlock = 1439;
        nPOWStartBlockInPOS = 259200;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 0; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = 21148191990 * COIN;
        nZerocoinStartHeight = 43200;
        nZerocoinStartTime = 1550037281;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1533859200;
        genesis.nNonce = 1742924;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000013ea705af4ed8b40a86250bc6d78b982c604eef80a908cec7f11abbdd1"));

        vFixedSeeds.clear();
        vSeeds.clear();

        vSeeds.push_back(CDNSSeedData("47.91.79.145", "47.91.79.145"));             // Germany node address
        vSeeds.push_back(CDNSSeedData("47.74.14.246", "47.74.14.246"));             // Japan node address
        vSeeds.push_back(CDNSSeedData("47.74.147.210", "47.74.147.210"));           // Singapore node address
        vSeeds.push_back(CDNSSeedData("47.90.215.200", "47.90.215.200"));           // US node address
        vSeeds.push_back(CDNSSeedData("47.89.243.161", "47.89.243.161"));           // Single node address

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 130); // Testnet ulo addresses start with 'u'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet eulo script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet eulo BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet eulo BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet eulo BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "042a5bab8709d72839c05e72a6396b69608a164abe94eeccdc311a9e0dd8fc8e3a13d75bcd2ffa6077ffea53319fa0e60ac0ba0bc6b392f7f296f0164d6ff08250";
        strObfuscationPoolDummyAddress = "uTxyz1tQzBkZrJaZgFQ9TXn2TxsSQQcoQ8";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0xec;
        pchMessageStart[2] = 0xf5;
        pchMessageStart[3] = 0xc5;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; // Eulo: 1 day
        nTargetSpacing = 1 * 60;        // Eulo: 1 minutes
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1533859200;
        genesis.nBits = 0x1f00ffff;
        genesis.nNonce = 63193;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 58806;
        assert(hashGenesisBlock == uint256("0x000078a088b16a4d3b943f8f7fb18dbd4d612e32c03b7dc25f17d8399db07924"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 58808;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
