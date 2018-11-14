#include "tcp_client.hpp"
#include <iostream>
#include <ESP8266WiFi.h>
//#include <ESP8266Client.h>
#include "global.h"
// char rcvBuff[1024];
// char trmBuff[1024];

#define ERROR_CALL(e, msg)   _error(e, msg, this, false)
#define ERROR_RECONN(e, msg) _error(e, msg, this, true)



namespace tcp
{
void default_error_callback(int error, const std::string &msg, TCPClient *client, bool reconnect)
{
  std::cout << " :: [TCP ERROR] :: " << error << " :: " << msg << std::endl;

  // Reconnect
  if (reconnect) {
    std::cout << " :: [TCP LOG] :: " << "try to reconnect to the server" << std::endl;
    client->reconnect();
  }
}

// Constructor
TCPClient::TCPClient() : //done
  connect_status(false),
  sock_number(-1)
{
  //pass
}

TCPClient::TCPClient(const std::string &address, const int &port, //done
                     error_callback ec) :
  connect_status(false),
  sock_number(-1)
{
  this->setup(address, port, ec);
}

// Destructor
TCPClient::~TCPClient() //done
{
  this->close();
}

// Utils
bool TCPClient::setup(const std::string &address, const int &port,
                      error_callback ec)
{

  if (connect_status == true)
    this->close();

  this->server_address = address;
  this->server_port = port;
  this->_error = ec;

  //Serial.print(server_address );
  //Serial.println(server_port);
  //Serial.print(server_port);



  // try to connect to the server
  return this->_connect();
}

// close socket
void TCPClient::close()
{
  if (this->connect_status == true)
    ::close(sock_number);
  this->connect_status = false;
}


bool TCPClient::reconnect()
{
  this->close();
  return this->_connect();
}

bool TCPClient::status()
{
  return this->connect_status;
}


// send/recv
//
bool TCPClient::send(std::string data)//done?
{
  if (connect_status == true)
  {
    // Serial.println("Data send:");
    //  Serial.print(data.c_str());
    /*           if(::send(sock_number, data.c_str(), data.size(), 0) < 0)
               {
                   ERROR_RECONN(CONNECTION_ERROR, "Failed to send message to the server");
                   return false;
               }*/
    // Serial.println("Data prep to send:");
    memcpy (trmBuff, data.c_str(), data.size());
    f_trmBuff();
  }
  else
    return false;
  return true;
}

bool TCPClient::recv(std::string &data, size_t size) //done?
{
  Serial.println("ASK Data recv:");
 // int count = f_alt_recv( data.c_str());
 // Serial.print(data.c_str());
  return true;
}

bool TCPClient::peek() //done
{
  /*       char buffer;
         if(recv( &buffer, MSG_PEEK | MSG_DONTWAIT) > 0)
         {
             return true;
         }
  */        return false;
}

bool TCPClient::getline(std::string &data, char delim)
{
  
       char buffer;
       std::string line;
       line.reserve(1024);

       char num_available;

       int count = 1;
       while(count > 0)
       {
         // Serial.println("Data rec line recv:");
          count = f_alt_recv( &buffer);
         // Serial.println(count);
         

           if(count < 0)
           {
               ERROR_RECONN(CONNECTION_ERROR, "Failed to getline from server");
              
               return false;
           }
           if(count > 0)
           {
               if(buffer != delim)
                   line.push_back(buffer);
               else
                   break;
           }
       }

       data = line;
// Serial.println("Data recv and readed:");
// Serial.println(data.c_str());
  return true;
}


//Get Func
std::string TCPClient::getAddress()   {
  return server_address;
}
int         TCPClient::getPort()      {
  return server_port;
}


// overload
TCPClient::operator bool()
{
  return this->connect_status;
}

// private function
bool TCPClient::_connect() //done WiFi and server connected
{




  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  /*  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  */
 // Serial.printf("WiFi Connected!\n");
  return this->connect_status = true;
  /*
    if(!client.connect(this->server_address, this->server_port)){
      Serial.println("Connect Failed");
      return;
    }
  */

}

}

#undef ERROR_CALL
#undef ERROR_RECONN
