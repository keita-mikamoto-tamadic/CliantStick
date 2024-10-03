/*******************************************************************************
 * File Name    : udp_client.cpp
 * Description  : LQR制御の実装
 ******************************************************************************
 * 変更履歴 : DD.MM.YYYY Version Description
 *          : 2024.07.19 1.00 K.Mikamoto 新設
 *****************************************************************************/

/*******************************************************************************
 * Includes
 *****************************************************************************/
#include "udp_client.h"

#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiUdp.h>

/*******************************************************************************
 * Global variables and functions
 *****************************************************************************/
void udp_client_open(void);
void udp_client_send(int data);
void udp_client_wifi_conCheck(void);

WiFiUDP udp;
st_udp_client g_udp_client;

/******************************************************************************
 * File Scope variables and functions
 *****************************************************************************/
static void client_setup(void);
static void connect_to_wifi(void);
static void wifi_event(WiFiEvent_t event);
static void reset_all_udp(void);
CRGB dispColor_udp_client(uint8_t g, uint8_t r, uint8_t b);

static const char *ssid_ = USER_SSID;
static const char *pass_ = PASSWORD;

static const char *toUdpAddr = "192.168.4.1";  // 送信先IP

static const int toUdpPort = 55555;  // 送信先ポート番号
static const int myUdpPort = 55556;  // 自ポート番号

typedef struct {
  bool isSetSendUdpPrev;
} st_prv;
static st_prv prv;

/******************************************************************************
 * Function Name: dispColor_udp_client
 * Description  : RGB値からCRGB型の色を生成
 * Arguments    : uint8_t g - 緑の輝度値
 *                uint8_t r - 赤の輝度値
 *                uint8_t b - 青の輝度値
 * Return Value : CRGB - 結合されたRGB値を持つCRGB型
 *****************************************************************************/
CRGB dispColor_udp_client(uint8_t g, uint8_t r, uint8_t b) {
  return (CRGB)((g << 16) | (r << 8) | b);
}

/******************************************************************************
 * Function Name: udp_client_send
 * Description  : 指定されたデータをUDPで送信
 * Arguments    : int data - 送信データ
 * Return Value : none
 *****************************************************************************/
void udp_client_send(int data) {
  st_udp_client *p_udp_client = &g_udp_client;
  st_prv *p_prv = &prv;

  if (p_udp_client->isSetSendUdp == true) {
    udp.beginPacket(toUdpAddr, toUdpPort);
    udp.write(data);
    // Serial.printf("send_position=%d, send_color_num=%d\r\n", send_position,
    // send_color_num);
    udp.endPacket();
    p_udp_client->isSetSendUdp = false;
  }
}

/******************************************************************************
 * Function Name: udp_client_open
 * Description  : UDPクライアントのセットアップとWi-Fi接続
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
void udp_client_open(void) {
  st_udp_client *p_udp_client = &g_udp_client;

  client_setup();
  connect_to_wifi();
  while (!p_udp_client->connected) {
    M5.dis.drawpix(0, dispColor_udp_client(0, 0, 0xFF));
    delay(1);
  }
}

/******************************************************************************
 * Function Name: client_setup
 * Description  : シリアル通信を初期化し、UDPクライアントの状態をリセット
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
static void client_setup(void) {
  Serial.begin(115200);

  // 変数初期化
  reset_all_udp();
  delay(1000);
}

/******************************************************************************
 * Function Name: connect_to_wifi
 * Description  : Wi-Fiネットワークへの接続を初期化し、接続イベントを設定
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
static void connect_to_wifi(void) {
  Serial.println("Connecting to WiFi network: " + String(USER_SSID));
  //  Do initialize
  WiFi.disconnect(true, true);

  delay(1000);

  WiFi.onEvent(wifi_event);
  WiFi.begin(USER_SSID, PASSWORD);
  Serial.println("Waiting for WIFI connection...");
}

/******************************************************************************
 * Function Name: wifi_event
 * Description  : Wi-Fiイベントに応じた処理を行い、接続状態を管理
 * Arguments    : WiFiEvent_t event - Wi-Fiイベントのタイプ
 * Return Value : none
 *****************************************************************************/
static void wifi_event(WiFiEvent_t event) {
  IPAddress myIP = WiFi.localIP();
  st_udp_client *p_udp_client = &g_udp_client;

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected!");
      Serial.print("My IP address: ");
      Serial.println(myIP);
      // udp.begin関数は自サーバーの待ち受けポート開放する関数である
      udp.begin(myIP, myUdpPort);
      delay(1000);
      p_udp_client->connected = true;

      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      p_udp_client->connected = false;

      break;
    default:
      break;
  }
}

/******************************************************************************
 * Function Name: udp_client_wifi_conCheck
 * Description  : Wi-Fi接続状態を確認し、切断されている場合は再接続
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
void udp_client_wifi_conCheck(void) {
  st_udp_client *p_udp_client = &g_udp_client;

  if (WiFi.status() != WL_CONNECTED) {
    connect_to_wifi();
    while (!p_udp_client->connected) {
      M5.dis.drawpix(0, dispColor_udp_client(0, 0, 0xFF));
      delay(1);
    }
    M5.dis.drawpix(0, dispColor_udp_client(0, 0, 0));
  }
}

/******************************************************************************
 * Function Name: reset_all_udp
 * Description  : UDPクライアントの接続状態と送信設定を初期化
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
static void reset_all_udp(void) {
  st_udp_client *p_udp_client = &g_udp_client;

  p_udp_client->connected = false;
  p_udp_client->isSetSendUdp = false;
}
