#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

// Macro define
#define USER_SSID "aaatamadic"
#define PASSWORD "aaatamadic"

// Global variables
typedef struct
{
  bool connected;
  bool isSetSendUdp;

}st_udp_client;
extern st_udp_client g_udp_client;

// Global functions
extern void udp_client_open(void);
extern void udp_client_send(int data);

extern void udp_client_wifi_conCheck(void);

#endif // _UDP_CLIENT_H_
