#include "btc_block.hpp"
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace btc
{
/////////////////// BTCBlock ////////////////
//

BTCBlock::BTCBlock() :
  clean_job(false)
{ /*
     Serial.printl( this->job_id);
     Serial.printl(this->nprevhash);
     Serial.printl( this->coinbase1);
     Serial.printl( this->coinbase2);
     Serial.printl( this->merkle_branch);
     Serial.printl( this->version);
     Serial.printl( this->nbits);
     Serial.printl( this->ntime);
     Serial.printl( this->extranonce1);
     Serial.printl( this->extranonce2);
     Serial.printl( this->nonce);
     Serial.printl( this->clean_job);
     Serial.printl( this->difficulty);
     Serial.printl( this->extranonce2_size);
  */
}

BTCBlock BTCBlock::None()
{
  return BTCBlock();
}


void BTCBlock::hash(char *version_, char *prevhash_,char *ntime_,char *nbits_, char *nonse_, int tx_, char **merkle_branch)
{

  
solve(version_, prevhash_, ntime_, nbits_, nonse_, tx_ , merkle_branch);

  
  //this->nonce.append("00000001");
  this->nonce.append(nonse_);
  this->extranonce2.append("00000000");

}
bool BTCBlock::valid()
{
  return job_id != "";
}

}
