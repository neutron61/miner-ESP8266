
#include <iostream>


#include "btc_pool.hpp"
#include "global.h"
#include "tcp_client.hpp"
#include <Wire.h>
#include "OLEDDisplay.h" 
#include "SH1106.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  

//------defines for tcp----------------------------
#define SSID_STR "stratum.slushpool.com"
#define PASSWORD "slushpool"
#define LOGIN "gggabriella.worker1"
#define TCP_PORT 3333

#define WifiSSID "homenet2"
#define WifiPASSWORD "25012010"

#define DNS_PORT 53
#include <ESPAsyncTCP.h>

extern "C" {
#include <osapi.h>
#include <os_type.h>
#include "user_interface.h"
}

#include "config.h"

AsyncClient* client = new AsyncClient;

char rcvBuff[4096];
int rcv_l;
char trmBuff[1024];

bool connected_to_pool;
bool wait_server;
bool init_miner;

#include <Ticker.h>
Ticker tickerSet;
unsigned int cur_nonce;
unsigned int cur_nonce_old;
int read_enable;
int second_counter;
char  check_hash[32];
int switchPin = SW_PIN_NUMBER;
unsigned int feed_nonce;

// Pins based on your wiring
#define SCL_PIN D5
#define SDA_PIN D3
SH1106 display(0x3c, SDA_PIN, SCL_PIN);

int status_miner;
//-------------------------------------------------
//-------------global------------------------------
btc::StratumPool pool(SSID_STR, TCP_PORT,  LOGIN, PASSWORD);

btc::BTCBlock block;
//int buf_read_count;

//-------------------------------------------------

void f_trmBuff()
{

  client->add(trmBuff, strlen(trmBuff));
  client->send();
  //Serial.println("Snd trmBuff:");
  //Serial.println(trmBuff);
  //  strcpy(rcvBuff, line.c_str());

  delay(2000);

}


int f_alt_recv(char *data)
{

  if (rcv_l > 0) {
    data[0] = rcvBuff[0];
    for (int n = 0 ; n < sizeof(rcvBuff) - 1 ; n++)
      rcvBuff[0 + n] = rcvBuff[1 + n];


    rcv_l = rcv_l - 1;
    return rcv_l;
  }
  else
    return 0;
}


static void replyToServer(void* arg) {

  connected_to_pool = true;

}

/* event callbacks */
static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
  /*
    Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
    Serial.write((uint8_t*)data, len);
  */
  char buf[len + 1];
  strncpy (buf, (char*)data, sizeof (buf));
  for (int n = rcv_l; n < len + rcv_l; n++)
    rcvBuff[n] = buf[n - rcv_l];
  /*
    // line += (char*)data;
    line.replace(rcv_l, len, (char*)data);*/
  rcv_l += len;
  /* Serial.println("Recv by ESP and stored ");
    // Serial.println(line.c_str());
    Serial.println(rcv_l);*/

}

void onConnect(void* arg, AsyncClient* client) {
  //  Serial.printf("\n client has been connected to %s on port %d \n", SSID_STR, TCP_PORT);
  replyToServer(client);
}


void setup() {
  Serial.begin(115200);
  
  status_miner = STATUS_WIFI_CONNECT;
  cur_nonce = 0;
  pinMode(switchPin, INPUT_PULLUP);
  delay(2000);

int poop = 666;
  // Initialising the display
  display.init();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 6, String(poop));
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 24, F("LITS MARKETS"));
  display.display();


  connected_to_pool = false;
  wait_server = false;
  init_miner = false;
  // buf_read_count = 0;
  // line.reserve(4096);
  for (int n = 0; n < sizeof(rcvBuff); n++)
    rcvBuff[n] = 0;

  // connects to access point
 // WiFiManager wifiManager;
 // wifiManager.autoConnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WifiSSID, WifiPASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("Wifi connected");
  status_miner = STATUS_WIFI_CONNECTED;

  client->onData(&handleData, client);
  client->onConnect(&onConnect, client);
  client->connect(SSID_STR, TCP_PORT);

  // btc::StratumPool pool(SSID_STR, TCP_PORT,  LOGIN, PASSWORD);
  // btc::BTCBlock block;

tickerSet.attach_ms(SPEED_CYCLE, Tick, 1);


}

void displayMessage(String message){
 /* display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128, message);
  display.display();*/
}
void Tick(int n)
{

  switch (status_miner)
  {

    case STATUS_WIFI_CONNECT:
      Serial.println("STATUS_WIFI_CONNECT");
      displayMessage("STATUS_WIFI_CONNECT");
      break;

    case STATUS_WIFI_CONNECTED:
      Serial.println("STATUS_WIFI_CONNECTED");
       displayMessage("STATUS_WIFI_CONNECTED");
      break;

    case STATUS_POOL_DATA_EXC:
      Serial.println("STATUS_POOL_DATA_EXC");
       displayMessage("STATUS_POOL_DATA_EXC");
      break;

    case STATUS_MAIN_WORK:
      
      unsigned long start, ended, delta;
    
      Serial.println("STATUS_MAIN_WORK");
      displayMessage("STATUS_MAIN_WORK");
      start = micros();
      WiFi.disconnect();
      unsigned int nonce = random(4294967295);
      feed_nonce = nonce;

      String nonce_str;
      nonce_str = "nonce: ";
      nonce_str = String(nonce);
      Serial.println("nonce:");
      Serial.println(nonce);
       displayMessage("nonce");

      if (digitalRead(switchPin)) 
      {
        if (work(nonce)){
          Serial.println("Nonce FINDED!!!");
           displayMessage("Nonce FINDED!!!");}
        else
          {Serial.println("Fail");
           displayMessage("Fail");
          }
      }
   
      ended = micros();
      delta = ended - start;
      Serial.println(delta); 
      Serial.println((double)(1000000/delta)*(1000/SPEED_CYCLE));
   /*  
      displayMessage("STATUS_WIFI_CONNECT");
      Serial.println("Looking for next nonce:");
      Serial.println(nonce+1);
*/
      break;
  }


}
int work(unsigned int nonce)
{
  
block = pool.getNewBlock();
return cur_nonce;

}
void loop() {
  ESP.wdtFeed();
  // yield();

  if (connected_to_pool) {
//ESP.wdtFeed();
    if (!init_miner)
    {
      btc::StratumPool pool(SSID_STR, TCP_PORT,  LOGIN, PASSWORD);
      btc::BTCBlock block;
      init_miner = true;
    }


    if (pool.detectNewBlock())
    {
   //   ESP.wdtFeed();
      block = pool.getNewBlock();
    }
    if (status_miner !=  STATUS_MAIN_WORK)
    {
   //   ESP.wdtFeed();
      status_miner =  STATUS_POOL_DATA_EXC;
      //solve your block
   //   ESP.wdtFeed();
      pool.submit(block);
   //   ESP.wdtFeed();
    }
  }
  else
  {

  }

}

char *convert(const std::string & s)
{
  char *pc = new char[s.size() + 1];
  std::strcpy(pc, s.c_str());
  return pc;
}


namespace std {

void __throw_bad_cast(void)
{
  // print error and stop execution
}
void __throw_ios_failure(char const*) {}
void __throw_runtime_error(char const*) {}


} // namespace std
