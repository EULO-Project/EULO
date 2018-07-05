#include "tmpblocksmempool.h"
#include "main.h"

TmpBlocksMempool::TmpBlocksMempool()
{

}


bool TmpBlocksMempool::HaveTmpBlock(const uint256& ori_hash_) const
{
    std::map<uint256, std::pair<CTmpBlockParams,int64_t>>::const_iterator it = mapTmpBlock.find(ori_hash_);
    return (it != mapTmpBlock.end());
}


