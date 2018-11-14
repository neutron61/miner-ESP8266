#include <iostream>
#include <vector>
#include <string>
#include "global.h"

namespace btc
{
    struct BTCBlock
    {
        std::string job_id;
        std::string prevhash;
        std::string coinbase1;
        std::string coinbase2;
        std::vector<std::string> merkle_branch;
        std::string version;
        std::string nbits;
        std::string ntime;
        std::string extranonce1;
        std::string extranonce2;
        std::string nonce;
        bool clean_job;
        unsigned int difficulty;
        int extranonce2_size;

        BTCBlock();
        bool valid();
        static BTCBlock None();
        void hash(char *version_, char *prevhash_,char *ntime_,char *nbits_, char *nonse_, int tx_, char **merkle_branch);
    };
}
