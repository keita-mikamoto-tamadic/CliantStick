/*******************************************************************************
 * File Name    : main.cpp
 * Description  : メインプログラムの実装
 ******************************************************************************
 * 変更履歴 : DD.MM.YYYY Version Description
 *          : 2024.04.04 1.00 K.Mikamoto 新設
 *****************************************************************************/

/*******************************************************************************
 * Includes
 *****************************************************************************/
#include <Arduino.h>
#include <M5Atom.h>

#include "udp_client.h"

/******************************************************************************
 * Macro definitions
 *****************************************************************************/
#define PIN_22 22  // LED Output
#define PIN_19 19  // LED Switch input
#define PIN_23 23  // trank move action to server
#define PIN_33 33  // trank close action to server

#define IDLE_LOOP 0x01
#define MOVE_TRANK 0x02
#define CLOSE_TRANK 0x03

/*******************************************************************************
 * Global variables and functions
 *****************************************************************************/

/******************************************************************************
 * File Scope variables and functions
 *****************************************************************************/
static void reset_all_main(void);
CRGB dispColor(uint8_t g, uint8_t r, uint8_t b);

// TaskHandle_t task_handl;

typedef struct {
  bool pinPrev;
} st_prv;
static st_prv prv;

/******************************************************************************
 * Function Name: setup
 * Description  : システムのセットアップ
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
void setup() {
  M5.begin(false, false, true);

  udp_client_open();

  M5.dis.drawpix(0, dispColor(0, 0, 0));

  reset_all_main();

  // INPUT
  pinMode(PIN_19, INPUT_PULLUP);
  pinMode(PIN_23, INPUT_PULLUP);
  pinMode(PIN_33, INPUT_PULLUP);

  // OUTPUT
  pinMode(PIN_22, OUTPUT);

  // initialize
  digitalWrite(PIN_22, LOW);
}

/******************************************************************************
 * Function Name: loop
 * Description  : メインループ処理
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
void loop() {
  st_prv *p_prv = &prv;
  st_udp_client *p_udp_client = &g_udp_client;

  static const uint8_t brightness = 0xFF;
  static bool pinState = false;

  // M5.dis.drawpix(0, dispColor(0, brightness, brightness));

  // 処理速度低下防止のためレジスタへのアクセスは最小限にする構造で実装すること
  // LED turn ON
  if (digitalRead(PIN_19) == 0) {
    pinState = true;
    if (pinState != p_prv->pinPrev) {
      // LED HIGH
      digitalWrite(PIN_22, HIGH);
      p_prv->pinPrev = true;
    }
  } else {
    pinState = false;
    if (pinState != p_prv->pinPrev) {
      // LED LOW
      digitalWrite(PIN_22, LOW);
      p_prv->pinPrev = false;
    }
  }

  // Command send move trank to server
  if (digitalRead(PIN_23) == 0) {
    p_udp_client->isSetSendUdp = true;
    udp_client_send(MOVE_TRANK);

  } else {
    p_udp_client->isSetSendUdp = true;
    udp_client_send(IDLE_LOOP);
  }

  // Command send close trank to server

  delay(10);

  // If wifi fails to connect, try reconnecting.
  udp_client_wifi_conCheck();
}

/******************************************************************************
 * Function Name: reset_all_main
 * Description  : メインループ処理
 * Arguments    : none
 * Return Value : none
 *****************************************************************************/
static void reset_all_main(void) {
  st_prv *p_prv = &prv;

  p_prv->pinPrev = false;
}

/******************************************************************************
 * Function Name: dispColor
 * Description  : RGB値からCRGB型の色を生成
 * Arguments    : uint8_t g - 緑の輝度値
 *                uint8_t r - 赤の輝度値
 *                uint8_t b - 青の輝度値
 * Return Value : CRGB - 結合されたRGB値を持つCRGB型
 *****************************************************************************/
CRGB dispColor(uint8_t g, uint8_t r, uint8_t b) {
  return (CRGB)((g << 16) | (r << 8) | b);
}