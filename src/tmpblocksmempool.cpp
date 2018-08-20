#include "tmpblocksmempool.h"
#include "main.h"

#ifdef  POW_IN_POS_PHASE

TmpBlocksMempool::TmpBlocksMempool()
{

}


bool TmpBlocksMempool::HaveTmpBlock(const uint256& hash) const
{
    std::map<uint256, std::pair<CTmpBlockParams,int64_t>>::const_iterator it = mapTmpBlock.find(hash);
    return (it != mapTmpBlock.end());
}

#endif
