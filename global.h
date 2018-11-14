#define STATUS_WIFI_CONNECT     0
#define STATUS_WIFI_CONNECTED   1
#define STATUS_POOL_DATA_EXC    2
#define STATUS_MAIN_WORK        3

#define SPEED_CYCLE             200
#define SW_PIN_NUMBER           0







extern int status_miner;
extern   char rcvBuff[4096];
extern  char trmBuff[1024];
void f_trmBuff();
int f_alt_recv(char *data);
void solve(char *version_, char *prevhash_,char *ntime_,char *nbits_, char *nonse_, int tx_, char **merkle_branch);
char *convert(const std::string & s);
extern unsigned int cur_nonce;
extern int read_enable;
extern char check_hash[32];
extern unsigned int feed_nonce;
