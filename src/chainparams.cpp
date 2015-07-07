// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The DynamicCoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
static Checkpoints::MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0, uint256("0x0000008b8a462251c714e1533ce9004f4a36e973673e70b99f54994588eae567"))
        ;
static const Checkpoints::CCheckpointData data = {
        &mapCheckpoints,
        1429025136, // * UNIX timestamp of last checkpoint block
        0,          // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        0           // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, uint256("0x000000ad85bc454622d3aff4842ef8f6f7e510a4c5f219c509ca701cac91e34e"))
        ;
static const Checkpoints::CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1429025142,
        0,
        0
    };

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0x000079f17b32013353791d669236ea05415ce722dcb4a1bb3c0efeb3e4740200"))
        ;
static const Checkpoints::CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xe0;
        vAlertPubKey = ParseHex("04db94e84b7bc99965a368efb0a7b7773715ff61d46bbfa27ecc5f30572e9e50191b58b3c53bd6564e1f9b0a0041b6e45d5124516e475fbd85c4245b8f22213141");
        nDefaultPort = 7333;
        bnProofOfWorkLimit = ~uint256(0) >> 24;
        nSubsidyHalvingInterval = 210000;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 3 * 60 * 60; // 3 hours in seconds
        nTargetSpacing = 15;    // seconds

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         */
        const char* pszTimestamp = "DynamicCoin genesis / MainNet";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 0 << bnProofOfWorkLimit.GetCompact() << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1024 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04d5097dbafdba541ea1e7000f70190a066fe006fdbc807944d51cb6fe3fdab435c1c342b72fc7b4b9b34e4809e24a9f988b0039f27b62ef7ac298ebd4350c13f2") << OP_CHECKSIG;
        genesis.vtx.resize(1);
        genesis.vtx[0] = txNew;
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 3;
        genesis.nTime    = 1429025136;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 5987573;
        hashGenesisBlock = genesis.GetHash();

        LogPrintf("%s: coinbaseTx.vin[0].scriptSig=%s\n", std::string(pszTimestamp), txNew.vin[0].scriptSig.ToString());

        assert(hashGenesisBlock == uint256("0x0000008b8a462251c714e1533ce9004f4a36e973673e70b99f54994588eae567"));
        assert(genesis.hashMerkleRoot == uint256("0x2f4b3c5a4ed656b563bf4197ed0b20a32b0cd793740d899e112b09de0ab61cca"));

        vFixedSeeds.clear(); //! No seeds for now
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("dynamiccoin.net", "main.seeds.dynamiccoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(0);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(5);
        base58Prefixes[SECRET_KEY] =     list_of(128);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4);

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
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
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x08;
        vAlertPubKey = ParseHex("04302390343f91cc401d56d68b123028bf52e5fca1939df127f63c6467cdf9c8e2c14b61104cf817d0b780da337893ecc4aaff1309e536162dabbdb45200ca2b0a");
        nDefaultPort = 17333;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 3 * 60 * 60; // 3 hours
        nTargetSpacing = 15;

        const char* pszTimestamp = "DynamicCoin genesis / TestNet";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 0 << bnProofOfWorkLimit.GetCompact() << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1024 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04d5097dbafdba541ea1e7000f70190a066fe006fdbc807944d51cb6fe3fdab435c1c342b72fc7b4b9b34e4809e24a9f988b0039f27b62ef7ac298ebd4350c13f2") << OP_CHECKSIG;
        genesis.vtx.resize(1);
        genesis.vtx[0] = txNew;
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 3;
        genesis.nTime    = 153469708;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 19102285;
        hashGenesisBlock = genesis.GetHash();

        LogPrintf("%s: coinbaseTx.vin[0].scriptSig=%s\n", std::string(pszTimestamp), txNew.vin[0].scriptSig.ToString());

        assert(hashGenesisBlock == uint256("0x000000ad85bc454622d3aff4842ef8f6f7e510a4c5f219c509ca701cac91e34e"));
        assert(genesis.hashMerkleRoot == uint256("0x5cf528874737db2abdcfc5be9093b172db4820672c1215e30142dbf1710b8b59"));

        vFixedSeeds.clear(); //! No seeds for now
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("dynamiccoin.net", "test.seeds.dynamiccoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(111);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(196);
        base58Prefixes[SECRET_KEY]     = list_of(239);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94);

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;
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
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xdb;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 3 * 60 * 60; //! 3 hours
        nTargetSpacing = 15;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nDefaultPort = 17444;

        const char* pszTimestamp = "DynamicCoin genesis / Reg test";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 0 << bnProofOfWorkLimit.GetCompact() << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1024 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04d5097dbafdba541ea1e7000f70190a066fe006fdbc807944d51cb6fe3fdab435c1c342b72fc7b4b9b34e4809e24a9f988b0039f27b62ef7ac298ebd4350c13f2") << OP_CHECKSIG;
        genesis.vtx.resize(1);
        genesis.vtx[0] = txNew;
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 3;
        genesis.nTime    = 1425370890;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 48961;
        hashGenesisBlock = genesis.GetHash();

        LogPrintf("%s: coinbaseTx.vin[0].scriptSig=%s\n", std::string(pszTimestamp), txNew.vin[0].scriptSig.ToString());

        assert(hashGenesisBlock == uint256("0x000079f17b32013353791d669236ea05415ce722dcb4a1bb3c0efeb3e4740200"));
        assert(genesis.hashMerkleRoot == uint256("0xe5cd902c5c1187e9e15a89b8a947c25c610436154c1542218d22bde80ee1ff9f"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
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
class CUnitTestParams : public CMainParams, public CModifiableParams {
public:
    CUnitTestParams() {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 17445;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Unit test mode doesn't have any DNS seeds.

        const char* pszTimestamp = "DynamicCoin genesis / Unit test";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 0 << bnProofOfWorkLimit.GetCompact() << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1024 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04d5097dbafdba541ea1e7000f70190a066fe006fdbc807944d51cb6fe3fdab435c1c342b72fc7b4b9b34e4809e24a9f988b0039f27b62ef7ac298ebd4350c13f2") << OP_CHECKSIG;
        genesis.vtx.resize(1);
        genesis.vtx[0] = txNew;
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 3;
        genesis.nTime    = 1429025148;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 6505914;
        hashGenesisBlock = genesis.GetHash();

        LogPrintf("%s: coinbaseTx.vin[0].scriptSig=%s\n", std::string(pszTimestamp), txNew.vin[0].scriptSig.ToString());

        assert(hashGenesisBlock == uint256("0x000000943d5dd598d7b49dd709db84c9c7bcdd37e4151ec7830d3877eb4b9ca5"));
        assert(genesis.hashMerkleRoot == uint256("0x453ea46287edbc1299c004dde772c676fd95c848cc794b9fa302cdc84bd1a176"));

        fRequireRPCPassword = false;
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
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval)  { nSubsidyHalvingInterval=anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority)  { nEnforceBlockUpgradeMajority=anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority)  { nRejectBlockOutdatedMajority=anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority)  { nToCheckBlockUpgradeMajority=anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks)  { fDefaultConsistencyChecks=afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) {  fAllowMinDifficultyBlocks=afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams *pCurrentParams = 0;

CModifiableParams *ModifiableParams()
{
   assert(pCurrentParams);
   assert(pCurrentParams==&unitTestParams);
   return (CModifiableParams*)&unitTestParams;
}

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
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

void SelectParams(CBaseChainParams::Network network) {
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
