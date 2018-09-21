#include "util.h"
#include "contractcomponent.h"
#include "contractbase.h"
#include "univalue/include/univalue.h"
#include "timedata.h"
#include "contractconfig.h"
#include "main.h"
#include "libdevcore/Address.h"
#include "libdevcore/Log.h"
#include <libethereum/LastBlockHashesFace.h>

static std::unique_ptr<EuloState> globalState;
static std::shared_ptr<dev::eth::SealEngineFace> globalSealEngine;
static StorageResults *pstorageresult = NULL;
static bool fRecordLogOpcodes = false;
static bool fIsVMlogFile = false;
static bool fGettingValuesDGP = false;

/** Too high fee. Can not be triggered by P2P transactions */
static const unsigned int REJECT_HIGHFEE = 0x100;

bool CheckMinGasPrice(std::vector<EthTransactionParams> &etps, const uint64_t &minGasPrice)
{
    for (EthTransactionParams &etp : etps)
    {
        if (etp.gasPrice < dev::u256(minGasPrice))
            return false;
    }
    return true;
}

valtype
GetSenderAddress(const CTransaction &tx, const CCoinsViewCache *coinsView, const std::vector<CTransaction> *blockTxs)
{
    CScript script;
    bool scriptFilled = false; //can't use script.empty() because an empty script is technically valid

    // First check the current (or in-progress) block for zero-confirmation change spending that won't yet be in txindex
    if (blockTxs)
    {
        for (auto btx : *blockTxs)
        {
            if (btx.GetHash() == tx.vin[0].prevout.hash)
            {
                script = btx.vout[tx.vin[0].prevout.n].scriptPubKey;
                scriptFilled = true;
                break;
            }
        }
    }
    if (!scriptFilled && coinsView)
    {
        // script = coinsView->AccessCoin(tx.vin[0].prevout.hash).out.scriptPubKey;
        script = coinsView->AccessCoins(tx.vin[0].prevout.hash)->vout[tx.vin[0].prevout.n].scriptPubKey;
        scriptFilled = true;
    }
    if (!scriptFilled)
    {

        CTransaction txPrevout;
        uint256 hashBlock;
        if (GetTransaction(tx.vin[0].prevout.hash, txPrevout, hashBlock, true))
        {
            script = txPrevout.vout[tx.vin[0].prevout.n].scriptPubKey;
        }
        else
        {
            LogPrint("Error ","fetching transaction details of tx %s. This will probably cause more errors",
                     tx.vin[0].prevout.hash.ToString().c_str());
            return valtype();
        }
    }

    CTxDestination addressBit;
    txnouttype txType = TX_NONSTANDARD;
    if (ExtractDestination(script, addressBit, &txType))
    {
        if ((txType == TX_PUBKEY || txType == TX_PUBKEYHASH) &&
                addressBit.type() == typeid(CKeyID))
        {
            CKeyID senderAddress(boost::get<CKeyID>(addressBit));
            return valtype(senderAddress.begin(), senderAddress.end());
        }
    }
    //prevout is not a standard transaction format, so just return 0
    return valtype();
}


UniValue vmLogToJSON(const ResultExecute &execRes, const CTransaction &tx, const CBlock &block)
{
    UniValue result(UniValue::VOBJ);


    int height = chainActive.Tip()->nHeight;

    if (tx != CTransaction())
        result.push_back(Pair("txid", tx.GetHash().GetHex()));
    result.push_back(Pair("address", execRes.execRes.newAddress.hex()));
    if (block.GetHash() != CBlock().GetHash())
    {
        result.push_back(Pair("time", block.GetBlockTime()));
        result.push_back(Pair("blockhash", block.GetHash().GetHex()));
        result.push_back(Pair("blockheight", height + 1));

    } else
    {
        result.push_back(Pair("time", GetAdjustedTime()));
        result.push_back(Pair("blockheight", height));
    }
    UniValue logEntries(UniValue::VARR);
    dev::eth::LogEntries logs = execRes.txRec.log();
    for (dev::eth::LogEntry log : logs)
    {
        UniValue logEntrie(UniValue::VOBJ);
        logEntrie.push_back(Pair("address", log.address.hex()));
        UniValue topics(UniValue::VARR);
        for (dev::h256 l : log.topics)
        {
            UniValue topicPair(UniValue::VOBJ);
            topicPair.push_back(Pair("raw", l.hex()));
            topics.push_back(topicPair);
            //TODO add "pretty" field for human readable data
        }
        UniValue dataPair(UniValue::VOBJ);
        dataPair.push_back(Pair("raw", HexStr(log.data)));
        logEntrie.push_back(Pair("data", dataPair));
        logEntrie.push_back(Pair("topics", topics));
        logEntries.push_back(logEntrie);
    }
    result.push_back(Pair("entries", logEntries));
    return result;
}

void writeVMlog(const std::vector<ResultExecute> &res, const CTransaction &tx = CTransaction(),
                const CBlock &block = CBlock())
{
    boost::filesystem::path euloDir = GetDataDir() / "vmExecLogs.json";
    std::stringstream ss;
    if (fIsVMlogFile)
    {
        ss << ",";
    } else
    {
        std::ofstream file(euloDir.string(), std::ios::out | std::ios::app);
        file << "{\"logs\":[]}";
        file.close();
    }

    for (size_t i = 0; i < res.size(); i++)
    {
        ss << vmLogToJSON(res[i], tx, block).write();
        if (i != res.size() - 1)
        {
            ss << ",";
        } else
        {
            ss << "]}";
        }
    }

    std::ofstream file(euloDir.string(), std::ios::in | std::ios::out);
    file.seekp(-2, std::ios::end);
    file << ss.str();
    file.close();
    fIsVMlogFile = true;
}



std::vector<ResultExecute> CallContract(const dev::Address &addrContract, std::vector<unsigned char> opcode,
                                        const dev::Address &sender = dev::Address(), uint64_t gasLimit = 0)
{
    CBlock block;
    CMutableTransaction tx;

    CBlockIndex *pTip = chainActive.Tip();

    CBlockIndex *pblockindex = mapBlockIndex[pTip->GetBlockHash()];

    ReadBlockFromDisk(block, pblockindex);
    block.nTime = GetAdjustedTime();
    block.vtx.erase(block.vtx.begin() + 1, block.vtx.end());

    EuloDGP euloDGP(globalState.get(), fGettingValuesDGP);
    uint64_t blockGasLimit = euloDGP.getBlockGasLimit(pTip->nHeight + 1);

    if (gasLimit == 0)
    {
        gasLimit = blockGasLimit - 1;
    }
    dev::Address senderAddress =
            sender == dev::Address() ? dev::Address("ffffffffffffffffffffffffffffffffffffffff") : sender;
    tx.vout.push_back(
                CTxOut(0, CScript() << OP_DUP << OP_HASH160 << senderAddress.asBytes() << OP_EQUALVERIFY << OP_CHECKSIG));
    block.vtx.push_back(CTransaction(tx));

    EuloTransaction callTransaction(0, 1, dev::u256(gasLimit), addrContract, opcode, dev::u256(0));
    callTransaction.forceSender(senderAddress);
    callTransaction.setVersion(VersionVM::GetEVMDefault());


    ByteCodeExec exec(block, std::vector<EuloTransaction>(1, callTransaction), blockGasLimit);
    exec.performByteCode(dev::eth::Permanence::Reverted);
    return exec.getResult();
}

class LastBlockHashes : public dev::eth::LastBlockHashesFace
{
public:
    dev::h256s precedingHashes(dev::h256 const& /* _mostRecentHash */) const override
    {
        return dev::h256s(256, dev::h256());
    }
    void clear() override {}
};

CContractComponent::CContractComponent()
{

}

CContractComponent::~CContractComponent()
{

}

bool CContractComponent::ComponentInitialize()
{
    LogPrintStr("initialize CContract component");
    return true;
}

bool CContractComponent::ContractInit()
{

    //FixME: Comment this is not right?

    //    ////////////////////////////////////////////////////////////////////// //eulo-vm
    //    dev::g_logPost = [&](std::string const &s, char const *c)
    //    { VMLog("%s : %s", s.c_str(), c); };
    //    //////////////////////////////////////////////////////////////////////

    if ((GetBoolArg("-dgpstorage", false) && GetBoolArg("-dgpevm", false)) ||
            (!GetBoolArg("-dgpstorage", false) && GetBoolArg("-dgpevm", false)) ||
            (!GetBoolArg("-dgpstorage", false) && !GetBoolArg("-dgpevm", false)))
    {
        fGettingValuesDGP = true;
    } else
    {
        fGettingValuesDGP = false;
    }


    LogPrintf("AppInitMain: fGettingValuesDGP = %d", fGettingValuesDGP);

    dev::eth::Ethash::init();
    boost::filesystem::path stateDir = GetDataDir() / CONTRACT_STATE_DIR;
    bool fStatus = boost::filesystem::exists(stateDir);
    const std::string dirEulo(stateDir.string());
    const dev::h256 hashDB(dev::sha3(dev::rlp("")));
    dev::eth::BaseState existstate = fStatus ? dev::eth::BaseState::PreExisting : dev::eth::BaseState::Empty;
    globalState = std::unique_ptr<EuloState>(
                new EuloState(dev::u256(0), EuloState::openDB(dirEulo, hashDB, dev::WithExisting::Trust), dirEulo,
                              existstate));
    dev::eth::ChainParams cp((dev::eth::genesisInfo(dev::eth::Network::euloMainNetwork)));
    globalSealEngine = std::unique_ptr<dev::eth::SealEngineFace>(cp.createSealEngine());

    pstorageresult = new StorageResults(stateDir.string());


    bool IsEnabled =  [&]()->bool{
            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();

    if (IsEnabled)
    {
        CBlockIndex *pTip = chainActive.Tip();
        CBlock block;
        if (!ReadBlockFromDisk(block, pTip))
        {
            LogPrint("ReadBlockFromDisk ","failed at %d, hash=%s", pTip->nHeight,
                     pTip->GetBlockHash().ToString());
            assert(0);
            return false;
        } else
        {
            uint256 hashStateRoot;
            uint256 hashUTXORoot;
            if(block.GetVMState(hashStateRoot, hashUTXORoot) != RET_VM_STATE_OK)
            {
                assert(0);
                LogPrintStr("GetVMState failed");
                return false;
            }else {
                globalState->setRoot(uintToh256(hashStateRoot));
                globalState->setRootUTXO(uintToh256(hashUTXORoot));
            }
        }
    } else
    {
        globalState->setRoot(dev::sha3(dev::rlp("")));
        globalState->setRootUTXO(uintToh256(DEFAULT_HASH_UTXO_ROOT));
        globalState->populateFrom(cp.genesisState);
    }

    globalState->db().commit();
    globalState->dbUtxo().commit();

    fRecordLogOpcodes = GetBoolArg("-record-log-opcodes", false);
    fIsVMlogFile = boost::filesystem::exists(GetDataDir() / "vmExecLogs.json");

    //FixMe: Check if this is necessary,is defined by config or command line argument
    //    if (!ifChainObj->IsLogEvents())
    //    {
    //        pstorageresult->wipeResults();
    //    }
    if (true)
    {
        pstorageresult->wipeResults();
    }


    return true;
}


bool CContractComponent::ComponentStartup()
{
    LogPrintStr("starting CContract component\n");

    return true;
}

bool CContractComponent::ComponentShutdown()
{
    LogPrintStr("shutdown CContract component");

    delete pstorageresult;
    pstorageresult = NULL;
    delete globalState.release();
    globalSealEngine.reset();
    return true;
}

uint64_t CContractComponent::GetMinGasPrice(int height)
{
    uint64_t minGasPrice = 1;

    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return 0;
    }

    EuloDGP euloDGP(globalState.get(), fGettingValuesDGP);
    globalSealEngine->setEuloSchedule(euloDGP.getGasSchedule(height));
    minGasPrice = euloDGP.getMinGasPrice(height);

    return minGasPrice;
}

uint64_t CContractComponent::GetBlockGasLimit(int height)
{
    uint64_t blockGasLimit = 1;

    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return 0;
    }

    EuloDGP euloDGP(globalState.get(), fGettingValuesDGP);
    globalSealEngine->setEuloSchedule(euloDGP.getGasSchedule(height));
    blockGasLimit = euloDGP.getBlockGasLimit(height);

    return blockGasLimit;
}

bool CContractComponent::AddressInUse(string contractaddress)
{
    //
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return false;
    }
    dev::Address addrAccount(contractaddress);
    return globalState->addressInUse(addrAccount);
}

bool CContractComponent::CheckContractTx(const CTransaction tx, const CAmount nFees,
                                         CAmount &nMinGasPrice, int &level,
                                         string &errinfo, const CAmount nAbsurdFee, bool rawTx)
{
    dev::u256 txMinGasPrice = 0;


    int height = chainActive.Tip()->nHeight + 1;

    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return false;
    }

    uint64_t minGasPrice = GetMinGasPrice(height);
    uint64_t blockGasLimit = GetBlockGasLimit(height);
    size_t count = 0;
    for (const CTxOut &o : tx.vout)
        count += o.scriptPubKey.HasOpCreate() || o.scriptPubKey.HasOpCall() ? 1 : 0;
    EuloTxConverter converter(tx, NULL);
    ExtractEuloTX resultConverter;
    if (!converter.extractionEuloTransactions(resultConverter))
    {
        level = 100;
        errinfo = "bad-tx-bad-contract-format";
        return false;
    }
    std::vector<EuloTransaction> euloTransactions = resultConverter.first;
    std::vector<EthTransactionParams> euloETP = resultConverter.second;

    dev::u256 sumGas = dev::u256(0);
    dev::u256 gasAllTxs = dev::u256(0);
    for (EuloTransaction euloTransaction : euloTransactions)
    {
        sumGas += euloTransaction.gas() * euloTransaction.gasPrice();

        if (sumGas > dev::u256(INT64_MAX))
        {
            level = 100;
            errinfo = "bad-tx-gas-stipend-overflow";
            return false;
        }

        if (sumGas > dev::u256(nFees))
        {
            level = 100;
            errinfo = "bad-txns-fee-notenough";
            return false;
        }

        if (txMinGasPrice != 0)
        {
            txMinGasPrice = std::min(txMinGasPrice, euloTransaction.gasPrice());
        } else
        {
            txMinGasPrice = euloTransaction.gasPrice();
        }
        VersionVM v = euloTransaction.getVersion();
        if (v.format != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-format";
            return false;
        }
        if (v.rootVM != 1)
        {
            level = 100;
            errinfo = "bad-tx-version-rootvm";
            return false;
        }
        if (v.vmVersion != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-vmversion";
            return false;
        }
        if (v.flagOptions != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-flags";
            return false;
        }

        //check gas limit is not less than minimum mempool gas limit
        if (euloTransaction.gas() < GetBoolArg("-minmempoolgaslimit", MEMPOOL_MIN_GAS_LIMIT))
        {
            level = 100;
            errinfo = "bad-tx-too-little-mempool-gas";
            return false;
        }

        //check gas limit is not less than minimum gas limit (unless it is a no-exec tx)
        if (euloTransaction.gas() < MINIMUM_GAS_LIMIT && v.rootVM != 0)
        {
            level = 100;
            errinfo = "bad-tx-too-little-gas";
            return false;
        }

        if (euloTransaction.gas() > UINT32_MAX)
        {
            level = 100;
            errinfo = "bad-tx-too-much-gas";
            return false;
        }

        gasAllTxs += euloTransaction.gas();
        if (gasAllTxs > dev::u256(blockGasLimit))
        {
            level = 1;
            errinfo = "bad-txns-gas-exceeds-blockgaslimit";
            return false;
        }

        //don't allow less than DGP set minimum gas price to prevent MPoS greedy mining/spammers
        if (v.rootVM != 0 && (uint64_t)euloTransaction.gasPrice() < minGasPrice)
        {
            level = 100;
            errinfo = "bad-tx-low-gas-price";
            return false;
        }
    }

    if (!CheckMinGasPrice(euloETP, minGasPrice))
    {
        level = 100;
        errinfo = "bad-txns-small-gasprice";
        return false;
    }

    if (count > euloTransactions.size())
    {
        level = 100;
        errinfo = "bad-txns-incorrect-format";
        return false;
    }

    if (rawTx && nAbsurdFee && dev::u256(nFees) > dev::u256(nAbsurdFee) + sumGas)
    {
        level = REJECT_HIGHFEE;
        errinfo = "absurdly-high-fee" + strprintf("%d > %d", nFees, nAbsurdFee);
        return false;
    }

    nMinGasPrice = CAmount(txMinGasPrice);

    return true;
}

bool CContractComponent::RunContractTx(CTransaction tx, CCoinsViewCache *v, CBlock *pblock,
                                       uint64_t minGasPrice,
                                       uint64_t hardBlockGasLimit,
                                       uint64_t softBlockGasLimit,
                                       uint64_t txGasLimit,
                                       uint64_t usedGas,
                                       ByteCodeExecResult &testExecResult)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return false;
    }

    EuloTxConverter convert(tx, v, &pblock->vtx);

    ExtractEuloTX resultConverter;
    if (!convert.extractionEuloTransactions(resultConverter))
    {
        //this check already happens when accepting txs into mempool
        //therefore, this can only be triggered by using raw transactions on the staker itself
        return false;
    }
    std::vector<EuloTransaction> euloTransactions = resultConverter.first;
    dev::u256 txGas = 0;
    for (EuloTransaction euloTransaction : euloTransactions)
    {
        txGas += euloTransaction.gas();
        if (txGas > txGasLimit)
        {
            // Limit the tx gas limit by the soft limit if such a limit has been specified.
            return false;
        }

        if (usedGas + euloTransaction.gas() > softBlockGasLimit)
        {
            //if this transaction's gasLimit could cause block gas limit to be exceeded, then don't add it
            return false;
        }
        if (euloTransaction.gasPrice() < minGasPrice)
        {
            //if this transaction's gasPrice is less than the current DGP minGasPrice don't add it
            return false;
        }
    }
    // We need to pass the DGP's block gas limit (not the soft limit) since it is consensus critical.
    ByteCodeExec exec(*pblock, euloTransactions, hardBlockGasLimit);
    if (!exec.performByteCode())
    {
        //error, don't add contract
        return false;
    }

    if (!exec.processingResults(testExecResult))
    {
        return false;
    }
    return true;
}

const std::map<std::uint32_t, std::string> exceptionMap =
{
    {0,  "None"},
    {1,  "Unknown"},
    {2,  "BadRLP"},
    {3,  "InvalidFormat"},
    {4,  "OutOfGasIntrinsic"},
    {5,  "InvalidSignature"},
    {6,  "InvalidNonce"},
    {7,  "NotEnoughCash"},
    {8,  "OutOfGasBase"},
    {9,  "BlockGasLimitReached"},
    {10, "BadInstruction"},
    {11, "BadJumpDestination"},
    {12, "OutOfGas"},
    {13, "OutOfStack"},
    {14, "StackUnderflow"},
    {15, "CreateWithValue"},
};

uint32_t GetExcepted(dev::eth::TransactionException status)
{
    uint32_t index = 0;
    if (status == dev::eth::TransactionException::None)
    {
        index = 0;
    } else if (status == dev::eth::TransactionException::Unknown)
    {
        index = 1;
    } else if (status == dev::eth::TransactionException::BadRLP)
    {
        index = 2;
    } else if (status == dev::eth::TransactionException::InvalidFormat)
    {
        index = 3;
    } else if (status == dev::eth::TransactionException::OutOfGasIntrinsic)
    {
        index = 4;
    } else if (status == dev::eth::TransactionException::InvalidSignature)
    {
        index = 5;
    } else if (status == dev::eth::TransactionException::InvalidNonce)
    {
        index = 6;
    } else if (status == dev::eth::TransactionException::NotEnoughCash)
    {
        index = 7;
    } else if (status == dev::eth::TransactionException::OutOfGasBase)
    {
        index = 8;
    } else if (status == dev::eth::TransactionException::BlockGasLimitReached)
    {
        index = 9;
    } else if (status == dev::eth::TransactionException::BadInstruction)
    {
        index = 10;
    } else if (status == dev::eth::TransactionException::BadJumpDestination)
    {
        index = 11;
    } else if (status == dev::eth::TransactionException::OutOfGas)
    {
        index = 12;
    } else if (status == dev::eth::TransactionException::OutOfStack)
    {
        index = 13;
    } else if (status == dev::eth::TransactionException::StackUnderflow)
    {
        index = 14;
    } else if (status == dev::eth::TransactionException::CreateWithValue)
    {
        index = 15;
    }
    auto it = exceptionMap.find(index);
    if (it != exceptionMap.end())
    {
        return it->first;
    } else
    {
        return 0;
    }
}

string CContractComponent::GetExceptedInfo(uint32_t index)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return "";
    }
    auto it = exceptionMap.find(index);
    if (it != exceptionMap.end())
    {
        return it->second;
    } else
    {
        return "";
    }
}

bool CContractComponent::ContractTxConnectBlock(CTransaction tx, uint32_t transactionIndex, CCoinsViewCache *v,
                                                const CBlock &block,
                                                int nHeight,
                                                ByteCodeExecResult &bcer,
                                                bool bLogEvents,
                                                bool fJustCheck,
                                                std::map<dev::Address, std::pair<CHeightTxIndexKey, std::vector<uint256>>> &heightIndexes,
                                                int &level, string &errinfo)
{

    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return false;
    }

    uint64_t minGasPrice = GetMinGasPrice(nHeight + 1);
    uint64_t blockGasLimit = GetBlockGasLimit(nHeight + 1);
    uint64_t countCumulativeGasUsed = 0;
    uint64_t blockGasUsed = 0;

    EuloTxConverter convert(tx, v, &block.vtx);

    ExtractEuloTX resultConvertQtumTX;
    if (!convert.extractionEuloTransactions(resultConvertQtumTX))
    {
        level = 100;
        errinfo = "bad-tx-bad-contract-format";
        return false;
    }
    if (!CheckMinGasPrice(resultConvertQtumTX.second, minGasPrice))
    {
        level = 100;
        errinfo = "bad-tx-low-gas-price";
        return false;
    }

    dev::u256 gasAllTxs = dev::u256(0);
    ByteCodeExec exec(block, resultConvertQtumTX.first, blockGasLimit);
    //validate VM version and other ETH params before execution
    //Reject anything unknown (could be changed later by DGP)
    //TODO evaluate if this should be relaxed for soft-fork purposes
    bool nonZeroVersion = false;
    dev::u256 sumGas = dev::u256(0);
    CAmount nTxFee = v->GetValueIn(tx) - tx.GetValueOut();
    for (EuloTransaction &qtx : resultConvertQtumTX.first)
    {
        sumGas += qtx.gas() * qtx.gasPrice();

        if (sumGas > dev::u256(INT64_MAX))
        {
            level = 100;
            errinfo = "bad-tx-gas-stipend-overflow";
            return false;
        }

        if (sumGas > dev::u256(nTxFee))
        {
            level = 100;
            errinfo = "bad-txns-fee-notenough";
            return false;
        }

        VersionVM v = qtx.getVersion();
        if (v.format != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-format";
            return false;
        }
        if (v.rootVM != 0)
        {
            nonZeroVersion = true;
        } else
        {
            if (nonZeroVersion)
            {
                //If an output is version 0, then do not allow any other versions in the same tx
                level = 100;
                errinfo = "bad-tx-mixed-zero-versions";
                return false;
            }
        }
        if (!(v.rootVM == 0 || v.rootVM == 1))
        {
            level = 100;
            errinfo = "bad-tx-version-rootvm";
            return false;
        }
        if (v.vmVersion != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-vmversion";
            return false;
        }
        if (v.flagOptions != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-flags";
            return false;
        }
        //check gas limit is not less than minimum gas limit (unless it is a no-exec tx)
        if (qtx.gas() < MINIMUM_GAS_LIMIT && v.rootVM != 0)
        {
            level = 100;
            errinfo = "bad-tx-too-little-gas";
            return false;
        }
        if (qtx.gas() > UINT32_MAX)
        {
            level = 100;
            errinfo = "bad-tx-too-much-gas";
            return false;
        }
        gasAllTxs += qtx.gas();
        if (gasAllTxs > dev::u256(blockGasLimit))
        {
            level = 1;
            errinfo = "bad-txns-gas-exceeds-blockgaslimit";
            return false;
        }
        //don't allow less than DGP set minimum gas price to prevent MPoS greedy mining/spammers
        if (v.rootVM != 0 && (uint64_t)qtx.gasPrice() < minGasPrice)
        {
            level = 100;
            errinfo = "bad-tx-low-gas-price";
            return false;
        }
    }

    if (!nonZeroVersion)
    {
        //if tx is 0 version, then the tx must already have been added by a previous contract execution
        if (!tx.HasOpSpend())
        {
            level = 100;
            errinfo = "bad-tx-improper-version-0";
            return false;
        }
    }

    if (!exec.performByteCode())
    {
        level = 100;
        errinfo = "bad-tx-unknown-error";
        return false;
    }

    std::vector<ResultExecute> resultExec(exec.getResult());
    if (!exec.processingResults(bcer))
    {
        level = 100;
        errinfo = "bad-vm-exec-processing";
        return false;
    }

    countCumulativeGasUsed += bcer.usedGas;
    std::vector<TransactionReceiptInfo> tri;
    if (bLogEvents)
    {
        for (size_t k = 0; k < resultConvertQtumTX.first.size(); k++)
        {
            dev::Address key = resultExec[k].execRes.newAddress;
            if (!heightIndexes.count(key))
            {
                heightIndexes[key].first = CHeightTxIndexKey(nHeight, resultExec[k].execRes.newAddress);
            }
            heightIndexes[key].second.push_back(tx.GetHash());
            uint32_t excepted = GetExcepted(resultExec[k].execRes.excepted);
            tri.push_back(
                        TransactionReceiptInfo{block.GetHash(), uint32_t(nHeight), tx.GetHash(), uint32_t(transactionIndex),
                                               resultConvertQtumTX.first[k].from(), resultConvertQtumTX.first[k].to(),
                                               countCumulativeGasUsed, uint64_t(resultExec[k].execRes.gasUsed),
                                               resultExec[k].execRes.newAddress, resultExec[k].txRec.log(),
                                               excepted});
        }

        pstorageresult->addResult(uintToh256(tx.GetHash()), tri);
    }

    blockGasUsed += bcer.usedGas;
    if (blockGasUsed > blockGasLimit)
    {
        level = 1000;
        errinfo = "bad-blk-gaslimit";
        return false;
    }

    //eulo-vm
    if (fRecordLogOpcodes && !fJustCheck)
    {
        writeVMlog(resultExec, tx, block);
    }

    for (ResultExecute &re: resultExec)
    {
        if (re.execRes.newAddress != dev::Address() && !fJustCheck)
            LogPrint("Address : ","%s :", std::string(re.execRes.newAddress.hex()));
        //dev::g_logPost(std::string("Address : " + re.execRes.newAddress.hex()), NULL);
    }
    return true;
}

void CContractComponent::GetState(uint256 &hashStateRoot, uint256 &hashUTXORoot)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }
    dev::h256 oldHashStateRoot(globalState->rootHash()); // eulo-vm
    dev::h256 oldHashUTXORoot(globalState->rootHashUTXO()); // eulo-vm

    hashStateRoot = h256Touint(oldHashStateRoot);
    hashUTXORoot = h256Touint(oldHashUTXORoot);
}

void CContractComponent::UpdateState(uint256 hashStateRoot, uint256 hashUTXORoot)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }

    if (hashStateRoot.IsNull() || hashUTXORoot.IsNull())
    {
        return;
    }
    globalState->setRoot(uintToh256(hashStateRoot));
    globalState->setRootUTXO(uintToh256(hashUTXORoot));
}

void CContractComponent::DeleteResults(std::vector<CTransaction> const &txs)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->deleteResults(txs);
}

std::vector<TransactionReceiptInfo> CContractComponent::GetResult(uint256 const &hashTx)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return std::vector<TransactionReceiptInfo>();
    }
    return pstorageresult->getResult(uintToh256(hashTx));
}

void CContractComponent::CommitResults()
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->commitResults();
}

void CContractComponent::ClearCacheResult()
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->clearCacheResult();
}

std::map<dev::h256, std::pair<dev::u256, dev::u256>> CContractComponent::GetStorageByAddress(string address)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return std::map<dev::h256, std::pair<dev::u256, dev::u256>>();
    }

    dev::Address addrAccount(address);
    auto storage(globalState->storage(addrAccount));
    return storage;
};

void CContractComponent::SetTemporaryState(uint256 hashStateRoot, uint256 hashUTXORoot)
{

    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }
    if (hashStateRoot.IsNull() || hashUTXORoot.IsNull())
    {
        return;
    }
    TemporaryState ts(globalState);
    ts.SetRoot(uintToh256(hashStateRoot), uintToh256(hashUTXORoot));
}

std::unordered_map<dev::h160, dev::u256> CContractComponent::GetContractList()
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return std::unordered_map<dev::h160, dev::u256>();
    }
    auto map = globalState->addresses();
    return map;
};


CAmount CContractComponent::GetContractBalance(dev::h160 address)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return CAmount(0);
    }
    return CAmount(globalState->balance(address));
}

std::vector<uint8_t> CContractComponent::GetContractCode(dev::Address address)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return std::vector<uint8_t>();
    }
    return globalState->code(address);
}

bool CContractComponent::GetContractVin(dev::Address address, dev::h256 &hash, uint32_t &nVout, dev::u256 &value,
                                        uint8_t &alive)
{
    bool ret = false;
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return ret;
    }
    std::unordered_map<dev::Address, Vin> vins = globalState->vins();
    if (vins.count(address))
    {
        hash = vins[address].hash;
        nVout = vins[address].nVout;
        value = vins[address].value;
        alive = vins[address].alive;
        ret = true;
    }
    return ret;
}

UniValue executionResultToJSON(const dev::eth::ExecutionResult &exRes)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("gasUsed", CAmount(exRes.gasUsed)));
    std::stringstream ss;
    ss << exRes.excepted;
    result.push_back(Pair("excepted", ss.str()));
    result.push_back(Pair("newAddress", exRes.newAddress.hex()));
    result.push_back(Pair("output", HexStr(exRes.output)));
    result.push_back(Pair("codeDeposit", static_cast<int32_t>(exRes.codeDeposit)));
    result.push_back(Pair("gasRefunded", CAmount(exRes.gasRefunded)));
    result.push_back(Pair("depositSize", static_cast<int32_t>(exRes.depositSize)));
    result.push_back(Pair("gasForDeposit", CAmount(exRes.gasForDeposit)));
    return result;
}

UniValue transactionReceiptToJSON(const dev::eth::TransactionReceipt &txRec)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("stateRoot", txRec.stateRoot().hex()));
    result.push_back(Pair("gasUsed", CAmount(txRec.cumulativeGasUsed())));
    result.push_back(Pair("bloom", txRec.bloom().hex()));
    UniValue logEntries(UniValue::VARR);
    dev::eth::LogEntries logs = txRec.log();
    for (dev::eth::LogEntry log : logs)
    {
        UniValue logEntrie(UniValue::VOBJ);
        logEntrie.push_back(Pair("address", log.address.hex()));
        UniValue topics(UniValue::VARR);
        for (dev::h256 l : log.topics)
        {
            topics.push_back(l.hex());
        }
        logEntrie.push_back(Pair("topics", topics));
        logEntrie.push_back(Pair("data", HexStr(log.data)));
        logEntries.push_back(logEntrie);
    }
    result.push_back(Pair("log", logEntries));
    return result;
}

void CContractComponent::RPCCallContract(UniValue &result, const string addrContract, std::vector<unsigned char> opcode,
                                         string sender, uint64_t gasLimit)
{
    bool IsEnabled =  [&]()->bool{

            if(chainActive.Tip()== nullptr) return false;
            return chainActive.Tip()->IsContractEnabled();
}();
    if (!IsEnabled)
    {
        return;
    }

    dev::Address addrAccount(addrContract);
    dev::Address senderAddress(sender);

    std::vector<ResultExecute> execResults = CallContract(addrAccount, opcode, senderAddress, gasLimit);
    if (fRecordLogOpcodes)
    {
        writeVMlog(execResults);
    }
    result.push_back(Pair("executionResult", executionResultToJSON(execResults[0].execRes)));
    result.push_back(Pair("transactionReceipt", transactionReceiptToJSON(execResults[0].txRec)));
}

bool ByteCodeExec::performByteCode(dev::eth::Permanence type)
{
    for (EuloTransaction &tx : txs)
    {
        //validate VM version
        if (tx.getVersion().toRaw() != VersionVM::GetEVMDefault().toRaw())
        {
            return false;
        }
        LastBlockHashes lastBlockHashes;
        dev::eth::BlockHeader blockHeader{initBlockHeader()};
        dev::eth::EnvInfo envInfo(blockHeader, lastBlockHashes, 0);
        if (!tx.isCreation() && !globalState->addressInUse(tx.receiveAddress()))
        {
            LogPrintStr("performByteCode execption====="); //eulo debug
            dev::eth::ExecutionResult execRes;
            execRes.excepted = dev::eth::TransactionException::Unknown;
            result.push_back(ResultExecute{execRes, dev::eth::TransactionReceipt(dev::h256(), dev::u256(),
                                           dev::eth::LogEntries()),
                                           CTransaction()});
            continue;
        }
        LogPrintStr("performByteCode start exec====="); //eulo debug
        result.push_back(globalState->execute(envInfo, *globalSealEngine.get(), tx, type, OnOpFunc()));
    }
    globalState->db().commit();
    globalState->dbUtxo().commit();
    globalSealEngine.get()->deleteAddresses.clear();
    return true;
}

bool ByteCodeExec::processingResults(ByteCodeExecResult &resultBCE)
{
    for (size_t i = 0; i < result.size(); i++)
    {
        uint64_t gasUsed = (uint64_t)result[i].execRes.gasUsed;
        if (result[i].execRes.excepted != dev::eth::TransactionException::None)
        {
            LogPrintStr("TransactionException != None"); //eulo debug
            if (txs[i].value() > 0)
            { //refund the value to sender
                CMutableTransaction tx;
                tx.vin.push_back(CTxIn(h256Touint(txs[i].getHashWith()), txs[i].getNVout(), CScript() << OP_SPEND));
                CScript script(CScript() << OP_DUP << OP_HASH160 << txs[i].sender().asBytes() << OP_EQUALVERIFY
                               << OP_CHECKSIG);
                tx.vout.push_back(CTxOut(CAmount(txs[i].value()), script));
                resultBCE.valueTransfers.push_back(CTransaction(tx));
            }
            resultBCE.usedGas += gasUsed;
        } else
        {
            if (txs[i].gas() > UINT64_MAX ||
                    result[i].execRes.gasUsed > UINT64_MAX ||
                    txs[i].gasPrice() > UINT64_MAX)
            {
                return false;
            }
            uint64_t gas = (uint64_t)txs[i].gas();
            uint64_t gasPrice = (uint64_t)txs[i].gasPrice();

            resultBCE.usedGas += gasUsed;
            int64_t amount = (gas - gasUsed) * gasPrice;
            if (amount < 0)
            {
                return false;
            }
            if (amount > 0)
            {
                CScript script(CScript() << OP_DUP << OP_HASH160 << txs[i].sender().asBytes() << OP_EQUALVERIFY
                               << OP_CHECKSIG);
                resultBCE.refundOutputs.push_back(CTxOut(amount, script));
                resultBCE.refundSender += amount;
            }
        }
        if (result[i].tx != CTransaction())
        {
            LogPrint("processingResults ","%d", i); //eulo debug
            resultBCE.valueTransfers.push_back(result[i].tx);
        }
    }
    return true;
}

dev::eth::BlockHeader ByteCodeExec::initBlockHeader()
{
    dev::eth::BlockHeader blockHeader;

    CBlockIndex *tip = chainActive.Tip();
    blockHeader.setNumber(tip->nHeight + 1);
    blockHeader.setTimestamp(block.nTime);
    blockHeader.setDifficulty(dev::u256(block.nBits));

    blockHeader.setGasLimit(blockGasLimit);
    //  FixMe: adjust the following code here must have a model in CBlock to identify contract system condition
//    if(block.IsProofOfStake() && block.howto??->IsContractEnabled()){
//        blockHeader.setAuthor(EthAddrFromScript(block.vtx[1].vout[1].scriptPubKey));
//    }else
    if (block.IsProofOfStake())
    {
        if (block.nVersion < SMART_CONTRACT_VERSION)
            blockHeader.setAuthor(EthAddrFromScript(block.vtx[1].vout[1].scriptPubKey));
        else
            blockHeader.setAuthor(EthAddrFromScript(block.vtx[1].vout[2].scriptPubKey));
    }
    else
    {
        blockHeader.setAuthor(EthAddrFromScript(block.vtx[0].vout[0].scriptPubKey));
    }
    return blockHeader;
}

dev::Address ByteCodeExec::EthAddrFromScript(const CScript &script)
{
    CTxDestination addressBit;
    txnouttype txType = TX_NONSTANDARD;
    if (ExtractDestination(script, addressBit, &txType))
    {
        if ((txType == TX_PUBKEY || txType == TX_PUBKEYHASH) &&
                addressBit.type() == typeid(CKeyID))
        {
            CKeyID addressKey(boost::get<CKeyID>(addressBit));
            std::vector<unsigned char> addr(addressKey.begin(), addressKey.end());
            LogPrint("ByteCodeExec::EthAddrFromScript ","%s", HexStr(addr.begin(), addr.end())); //eulo debug
            return dev::Address(addr);
        }
    }
    //if not standard or not a pubkey or pubkeyhash output, then return 0
    return dev::Address();
}


bool EuloTxConverter::extractionEuloTransactions(ExtractEuloTX &eulotx)
{
    std::vector<EuloTransaction> resultTX;
    std::vector<EthTransactionParams> resultETP;
    for (size_t i = 0; i < txBit.vout.size(); i++)
    {
        if (txBit.vout[i].scriptPubKey.HasOpCreate() || txBit.vout[i].scriptPubKey.HasOpCall())
        {
            if (receiveStack(txBit.vout[i].scriptPubKey))
            {
                EthTransactionParams params;
                if (parseEthTXParams(params))
                {
                    LogPrintStr("extractionEuloTransactions"); //eulo debug
                    resultTX.push_back(createEthTX(params, i));
                    resultETP.push_back(params);
                } else
                {
                    return false;
                }
            } else
            {
                return false;
            }
        }
    }
    eulotx = std::make_pair(resultTX, resultETP);
    return true;
}

bool EuloTxConverter::receiveStack(const CScript &scriptPubKey)
{
    EvalScript(stack, scriptPubKey, SCRIPT_EXEC_BYTE_CODE, BaseSignatureChecker(),  nullptr);
    if (stack.empty())
        return false;

    CScript scriptRest(stack.back().begin(), stack.back().end());
    stack.pop_back();

    opcode = (opcodetype)(*scriptRest.begin());
    if ((opcode == OP_CREATE && stack.size() < 4) || (opcode == OP_CALL && stack.size() < 5))
    {
        stack.clear();
        return false;
    }

    return true;
}

bool EuloTxConverter::parseEthTXParams(EthTransactionParams &params)
{
    try
    {
        dev::Address receiveAddress;
        valtype vecAddr;
        if (opcode == OP_CALL)
        {
            vecAddr = stack.back();
            stack.pop_back();
            receiveAddress = dev::Address(vecAddr);
        }
        if (stack.size() < 4)
            return false;

        if (stack.back().size() < 1)
        {
            return false;
        }
        valtype code(stack.back());
        stack.pop_back();
        uint64_t gasPrice = CScriptNum::vch_to_uint64(stack.back());
        stack.pop_back();
        uint64_t gasLimit = CScriptNum::vch_to_uint64(stack.back());
        stack.pop_back();
        if (gasPrice > INT64_MAX || gasLimit > INT64_MAX)
        {
            return false;
        }
        //we track this as CAmount in some places, which is an int64_t, so constrain to INT64_MAX
        if (gasPrice != 0 && gasLimit > INT64_MAX / gasPrice)
        {
            //overflows past 64bits, reject this tx
            return false;
        }
        if (stack.back().size() > 4)
        {
            return false;
        }
        VersionVM version = VersionVM::fromRaw((uint32_t)CScriptNum::vch_to_uint64(stack.back()));
        stack.pop_back();
        params.version = version;
        params.gasPrice = dev::u256(gasPrice);
        params.receiveAddress = receiveAddress;
        params.code = code;
        params.gasLimit = dev::u256(gasLimit);
        return true;
    }
    catch (const scriptnum_error &err)
    {
        LogPrintStr("Incorrect parameters to VM.");
        return false;
    }
}

EuloTransaction EuloTxConverter::createEthTX(const EthTransactionParams &etp, uint32_t nOut)
{
    EuloTransaction txEth;
    if (etp.receiveAddress == dev::Address() && opcode != OP_CALL)
    {
        txEth = EuloTransaction(txBit.vout[nOut].nValue, etp.gasPrice, etp.gasLimit, etp.code, dev::u256(0));
    } else
    {
        txEth = EuloTransaction(txBit.vout[nOut].nValue, etp.gasPrice, etp.gasLimit, etp.receiveAddress, etp.code,
                                dev::u256(0));
    }
    dev::Address sender(GetSenderAddress(txBit, view, blockTransactions));
    txEth.forceSender(sender);
    txEth.setHashWith(uintToh256(txBit.GetHash()));
    txEth.setNVout(nOut);
    txEth.setVersion(etp.version);

    return txEth;
}


