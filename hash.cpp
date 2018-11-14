#include <iostream>
//#include <fstream>
#include <string>

#include <cstdio>
#include <cstring>

#include <cassert>

#include <stdio.h>
#include <stdlib.h>

#include "sha256.h"
#include <ESP8266WiFi.h>
#include "global.h"

////////////////////////   Block   /////////////////////

typedef struct _block
{
  unsigned long long version;
  unsigned char prevhash[32];
  unsigned char merkle_root[32];
  unsigned int ntime;
  unsigned int nbits;
  //unsigned int nonce;
  unsigned int nonce;
} HashBlock;


////////////////////////   Utils   ///////////////////////

//convert one hex-codec char to binary
unsigned char decode(unsigned char c)
{
  switch (c)
  {
    case 'a':
      return 0x0a;
    case 'b':
      return 0x0b;
    case 'c':
      return 0x0c;
    case 'd':
      return 0x0d;
    case 'e':
      return 0x0e;
    case 'f':
      return 0x0f;
    case '0' ... '9':
      return c - '0';
  }
}


// convert hex string to binary
//
// in: input string
// string_len: the length of the input string
//      '\0' is not included in string_len!!!
// out: output bytes array
void convert_string_to_little_endian_bytes(unsigned char* out, char *in, size_t string_len)
{
  assert(string_len % 2 == 0);

  size_t s = 0;
  size_t b = string_len / 2 - 1;

  for (s, b; s < string_len; s += 2, --b)
  {
    out[b] = (unsigned char)(decode(in[s]) << 4) + decode(in[s + 1]);
  }
}

// print out binary array (from highest value) in the hex format
void print_hex(unsigned char* hex, size_t len)
{
  for (int i = 0; i < len; ++i)
  {
    printf("%032x", hex[i]);
  }
}


// print out binar array (from lowest value) in the hex format
void print_hex_inverse(unsigned char* hex, size_t len)
{
  if (len > 2)
    for (int i = len - 1; i >= 0; --i)
    {
      printf("%02x", hex[i]);
    }
}

void copy_hex_inverse(unsigned char* hex, char* out, size_t len)
{ int cx;
  if (len > 2)
    for (int i = len - 1; i >= 0; --i)
    {
       cx = snprintf ( out, len, "%02x",  hex[i] );
    }
}

int little_endian_bit_comparison(const unsigned char *a, const unsigned char *b, size_t byte_len)
{
  // compared from lowest bit
  for (int i = byte_len - 1; i >= 0; --i)
  {
    if (a[i] < b[i])
      return -1;
    else if (a[i] > b[i])
      return 1;
  }
  return 0;
}

void getline(char *str, size_t len, FILE *fp)
{

  int i = 0;
  while ( i < len && (str[i] = fgetc(fp)) != EOF && str[i++] != '\n');
  str[len - 1] = '\0';
}

////////////////////////   Hash   ///////////////////////

void double_sha256(SHA256 *sha256_ctx, unsigned char *bytes, size_t len)
{
  SHA256 tmp;
  sha256(&tmp, (BYTE*)bytes, len);
  sha256(sha256_ctx, (BYTE*)&tmp, sizeof(tmp));
}


////////////////////   Merkle Root   /////////////////////


// calculate merkle root from several merkle branches
// root: output hash will store here (little-endian)
// branch: merkle branch  (big-endian)
// count: total number of merkle branch
void calc_merkle_root(unsigned char *root, int count, char **branch)
{
  size_t total_count = count; // merkle branch
  unsigned char *raw_list = new unsigned char[(total_count + 1) * 32];
  unsigned char **list = new unsigned char*[total_count + 1];

  // copy each branch to the list
  for (int i = 0; i < total_count; ++i)
  {
    list[i] = raw_list + i * 32;
    //convert hex string to bytes array and store them into the list
    convert_string_to_little_endian_bytes(list[i], branch[i], 64);
  }

  list[total_count] = raw_list + total_count * 32;


  // calculate merkle root
  while (total_count > 1)
  {

    // hash each pair
    int i, j;

    if (total_count % 2 == 1) //odd,
    {
      memcpy(list[total_count], list[total_count - 1], 32);
    }

    for (i = 0, j = 0; i < total_count; i += 2, ++j)
    {
      // this part is slightly tricky,
      //   because of the implementation of the double_sha256,
      //   we can avoid the memory begin overwritten during our sha256d calculation
      // double_sha:
      //     tmp = hash(list[0]+list[1])
      //     list[0] = hash(tmp)
      double_sha256((SHA256*)list[j], list[i], 64);
    }

    total_count = j;
  }

  memcpy(root, list[0], 32);

  delete[] raw_list;
  delete[] list;
}


void solve(char *version_, char *prevhash_, char *ntime_, char *nbits_, char *nonse_, int tx_, char **merkle_branch_)
{

  // **** read data *****
  char version[9];
  char prevhash[65];
  char ntime[9];
  char nbits[9];
  int tx;
  char *raw_merkle_branch;
  char **merkle_branch;

  //getline(version, 9, fin);
  //getline(prevhash, 65, fin);
  //getline(ntime, 9, fin);
  //getline(nbits, 9, fin);
  //fscanf(fin, "%d\n", &tx);
  strncpy(version, version_, 9);
  strncpy(prevhash, prevhash_, 65);
  strncpy(ntime, ntime_, 9);
  strncpy(nbits, nbits_, 9);
  tx = tx_;


  raw_merkle_branch = new char [tx * 65];
  merkle_branch = new char *[tx];
  for (int i = 0; i < tx; ++i)
  {
    merkle_branch[i] = raw_merkle_branch + i * 65;
    strncpy(merkle_branch[i], merkle_branch_[i], 65);
    merkle_branch[i][64] = '\0';
  }

  // **** calculate merkle root ****

  unsigned char merkle_root[32];
  calc_merkle_root(merkle_root, tx, merkle_branch);

  printf("merkle root(little): ");
  print_hex(merkle_root, 32);
  printf("\n");

  printf("merkle root(big):    ");
  print_hex_inverse(merkle_root, 32);
  printf("\n");


  // **** solve block ****
  printf("Block info (big): \n");
  printf("  version:  %s\n", version);
  printf("  pervhash: %s\n", prevhash);
  printf("  merkleroot: "); print_hex_inverse(merkle_root, 32); printf("\n");
  printf("  nbits:    %s\n", nbits);
  printf("  ntime:    %s\n", ntime);
  printf("ntime long %d\n", sizeof(ntime));
  printf("  nonce:   %d\n\n",nonse_);

status_miner =  STATUS_MAIN_WORK;

  HashBlock block;
  /*
      // convert to byte array in little-endian
      convert_string_to_little_endian_bytes((unsigned char *)&block.version, version, 8);
      convert_string_to_little_endian_bytes(block.prevhash,                  prevhash,    64);
      memcpy(block.merkle_root, merkle_root, 32);
      convert_string_to_little_endian_bytes((unsigned char *)&block.nbits,   nbits,     8);
      convert_string_to_little_endian_bytes((unsigned char *)&block.ntime,   ntime,     8);
      block.nonce = 0;
  */

  // ********** calculate target value *********
  // calculate target value from encoded difficulty which is encoded on "nbits"
  unsigned int exp = block.nbits >> 24;
  unsigned int mant = block.nbits & 0xffffff;
  unsigned char target_hex[32] = {};
  unsigned int shift = 8 * (exp - 3);
  unsigned int sb = shift / 8;
  unsigned int rb = shift % 8;

  // little-endian
  target_hex[sb    ] = (mant << rb);
  target_hex[sb + 1] = (mant >> (8 - rb));
  target_hex[sb + 2] = (mant >> (16 - rb));
  target_hex[sb + 3] = (mant >> (24 - rb));


  //  printf("Target value (big): ");
  //  print_hex_inverse(target_hex, 32);
  //  printf("\n");


  // ********** check nonce **************

  SHA256 sha256_ctx;
  if (feed_nonce == 0)
    block.nonce = (unsigned int )nonse_;
  else
    block.nonce = feed_nonce;
   
    ESP.wdtFeed();
    double_sha256(&sha256_ctx, (unsigned char*)&block, sizeof(block));
    ESP.wdtFeed();
    if (little_endian_bit_comparison(sha256_ctx.b, target_hex, 32) < 0) // sha256_ctx < target_hex
    {
      printf("Found Solution!!\n");
      //  printf("hash #%10u (big): ", block.nonce);
      //  print_hex_inverse(sha256_ctx.b, 32);
      //   printf("\n\n");
      cur_nonce = 1;
    }else
    {
      cur_nonce = 0;
      
    }
 

  delete[] merkle_branch;
  delete[] raw_merkle_branch;
}
