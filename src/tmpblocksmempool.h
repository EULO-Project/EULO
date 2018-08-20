#ifndef TMPBLOCKSMEMPOOL_H
#define TMPBLOCKSMEMPOOL_H

#include "uint256.h"
#include "base58.h"

#include "primitives/block.h"

#ifdef  POW_IN_POS_PHASE

class TmpBlocksMempool
{
public:
    TmpBlocksMempool();


    std::map<uint256, std::pair<CTmpBlockParams,int64_t>> mapTmpBlock;


    bool HaveTmpBlock(const uint256& hash) const;


private:


};

#endif  //  POW_IN_POS_PHASE

#endif // TMPBLOCKSMEMPOOL_H
