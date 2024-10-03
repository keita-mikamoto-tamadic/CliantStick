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
CRGB dispColor(uint8_t g, uint8_t r, uint8_t b);

volatile bool button19Pressed = false;
volatile bool button23Pressed = false;

void IRAM_ATTR handleButton19Interrupt() {
  button19Pressed = digitalRead(PIN_19) == LOW;
}

void IRAM_ATTR handleButton23Interrupt() {
  button23Pressed = digitalRead(PIN_23) == LOW;
}

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

  // INPUT
  pinMode(PIN_19, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_19), handleButton19Interrupt, CHANGE);

  pinMode(PIN_23, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_23), handleButton23Interrupt, CHANGE);

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
  st_udp_client *p_udp_client = &g_udp_client;

  if (button19Pressed) {
    digitalWrite(PIN_22, HIGH);
  } else {
    digitalWrite(PIN_22, LOW);
  }

  if (button23Pressed) {
    p_udp_client->isSetSendUdp = true;
    udp_client_send(MOVE_TRANK);
  } else {
    p_udp_client->isSetSendUdp = true;
    udp_client_send(IDLE_LOOP);
  }

  // If wifi fails to connect, try reconnecting.
  udp_client_wifi_conCheck();

  delay(10);
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