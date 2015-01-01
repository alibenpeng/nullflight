// license foo

#include "common.h"
#include "stm32f0xx.h"
#include "misc.h"
#include "led.h"
#include "nf_gpio.h"
#include "spi.h"
#include "iface_nrf24l01.h"
#include "rx_bk2423.h"
#include "stm32f0xx.h"

// cleanflight drivers
#include "system.h"

// Bit vector from bit position
#define BV(bit) (1 << bit)

static u8 tx_addr[5];
static u8 packet[9];

void rx_deinit(void) {
  NRF24L01_FlushRx(); // Packet ID 1314
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x40); // Packet ID 1315
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x00);

}

void rx_init(void)
{

  rx_deinit();
  delay(1000);

  NRF24L01_Initialize();

  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 0
  NRF24L01_Activate(0x53); // Packet ID 1
  NRF24L01_ReadRegisterMulti(NRF24L01_08_OBSERVE_TX, tx_addr, 4); // We have to dump the data somewhere and tx_addr is not used yet
  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 3
  NRF24L01_Activate(0x53); // Packet ID 4
  //NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0b); // Packet ID 5
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, BV(NRF24L01_00_PRIM_RX) | BV(NRF24L01_00_EN_CRC) | BV(NRF24L01_00_PWR_UP)); // same thing, only more readable
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x3f); // Packet ID 6
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x3f); // Packet ID 7
  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03); // Packet ID 8
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x1f); // Packet ID 9
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, 0x3c); // Packet ID 10
  NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, 0x07); // Packet ID 11
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x07); // Packet ID 12
  NRF24L01_WriteReg(NRF24L01_08_OBSERVE_TX, 0x00); // Packet ID 13
  NRF24L01_WriteReg(NRF24L01_09_CD, 0x00); // Packet ID 14
  NRF24L01_WriteReg(NRF24L01_0C_RX_ADDR_P2, 0xc3); // Packet ID 15
  NRF24L01_WriteReg(NRF24L01_0D_RX_ADDR_P3, 0xc4); // Packet ID 16
  NRF24L01_WriteReg(NRF24L01_0E_RX_ADDR_P4, 0xc5); // Packet ID 17
  NRF24L01_WriteReg(NRF24L01_0F_RX_ADDR_P5, 0xc6); // Packet ID 18
  NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, 0x09); // Packet ID 19
  NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, 0x09); // Packet ID 20
  NRF24L01_WriteReg(NRF24L01_13_RX_PW_P2, 0x09); // Packet ID 21
  NRF24L01_WriteReg(NRF24L01_14_RX_PW_P3, 0x09); // Packet ID 22
  NRF24L01_WriteReg(NRF24L01_15_RX_PW_P4, 0x09); // Packet ID 23
  NRF24L01_WriteReg(NRF24L01_16_RX_PW_P5, 0x09); // Packet ID 24
  NRF24L01_WriteReg(NRF24L01_17_FIFO_STATUS, 0x00); // Packet ID 25
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (u8 *) "\x65\x65\x65\x65\x65", 5); // Packet ID 26
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR, (u8 *) "\x65\x65\x65\x65\x65", 5); // Packet ID 27
  NRF24L01_ReadReg(NRF24L01_1D_FEATURE); // Packet ID 28
  NRF24L01_Activate(0x73); // Packet ID 29
  NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3f); // Packet ID 30
  NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x07); // Packet ID 31
  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 32
  NRF24L01_Activate(0x53); // Packet ID 33
  NRF24L01_WriteRegisterMulti(NRF24L01_00_CONFIG, (u8 *) "\x40\x4b\x01\xe2", 4); // Packet ID 34
  NRF24L01_WriteRegisterMulti(NRF24L01_01_EN_AA, (u8 *) "\xc0\x4b\x00\x00", 4); // Packet ID 35
  NRF24L01_WriteRegisterMulti(NRF24L01_02_EN_RXADDR, (u8 *) "\xd0\xfc\x8c\x02", 4); // Packet ID 36
  NRF24L01_WriteRegisterMulti(NRF24L01_03_SETUP_AW, (u8 *) "\x99\x00\x39\x21", 4); // Packet ID 37
  NRF24L01_WriteRegisterMulti(NRF24L01_04_SETUP_RETR, (u8 *) "\xd9\x96\x82\x1b", 4); // Packet ID 38
  NRF24L01_WriteRegisterMulti(NRF24L01_05_RF_CH, (u8 *) "\x24\x06\x7f\xa6", 4); // Packet ID 39
  NRF24L01_WriteRegisterMulti(NRF24L01_0C_RX_ADDR_P2, (u8 *) "\x00\x12\x73\x00", 4); // Packet ID 40
  NRF24L01_WriteRegisterMulti(NRF24L01_0D_RX_ADDR_P3, (u8 *) "\x46\xb4\x80\x00", 4); // Packet ID 41

  // this is obviously wrong since the W_REGISTER instruction only takes up to 5 data bytes, but the original RX does this, so fuck it!
  NRF24L01_WriteRegisterMulti(NRF24L01_0E_RX_ADDR_P4, (u8 *) "\x41\x10\x04\x82\x20\x08\x08\xf2\x7d\xef\xff", 11); // Packet ID 42
  NRF24L01_WriteRegisterMulti(NRF24L01_04_SETUP_RETR, (u8 *) "\xdf\x96\x82\x1b", 4); // Packet ID 43
  NRF24L01_WriteRegisterMulti(NRF24L01_04_SETUP_RETR, (u8 *) "\xd9\x96\x82\x1b", 4); // Packet ID 44
  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 45
  NRF24L01_Activate(0x53); // Packet ID 46
  NRF24L01_FlushRx(); // Packet ID 47
  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 48
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x0e); // Packet ID 49
  NRF24L01_ReadReg(NRF24L01_00_CONFIG); // Packet ID 50
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0b); // Packet ID 51

}


void rx_bind(void) {
  // write addresses for bind packet
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (u8 *) "\x65\x65\x65\x65\x65", 5); // Packet ID 52
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR, (u8 *) "\x65\x65\x65\x65\x65", 5); // Packet ID 53

  // wait for bind packet
  while (!(NRF24L01_ReadReg(NRF24L01_07_STATUS) & 0x40)) {} // wait for RX_DR (0x40) bit to be set

  while (!(NRF24L01_ReadReg(NRF24L01_17_FIFO_STATUS) & 0x01)) { // wait for RX_EMPTY (0x01) bit to be set
    if (NRF24L01_ReadPlWid() == 0x09) {
      NRF24L01_ReadPayload(packet, 9); // Packet ID 109
    }
  }

  NRF24L01_FlushRx(); // Packet ID 117
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x40); // Clear RX_DR

  // the first four bytes of the packet are the address to use
  for (int i = 0; i < 4; i++) {
    tx_addr[i] = packet[i];
  }

  tx_addr[4] = 0xc1; // no idea what this means

  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, tx_addr, 5); // Packet ID 119
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR, tx_addr, 5); // Packet ID 120


  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 121
  NRF24L01_ReadReg(NRF24L01_07_STATUS); // Packet ID 122

  LED0_ON;
  LED1_ON;
  delay(1000);
}

void rx_search_channel(void)
{
  u8 channels[] = { 0x3c, 0x02, 0x21, 0x41, 0x0b, 0x4b };
  u8 i = 5;
  while (!(NRF24L01_ReadReg(NRF24L01_07_STATUS) & 0x40)) {
    if (i<5) {
      i++;
    } else {
      i=0;
    }
    NRF24L01_WriteReg(NRF24L01_05_RF_CH, channels[i]); // Packet ID 123
  }
}


void rx_read_payload(void)
{
  if ((NRF24L01_ReadReg(NRF24L01_07_STATUS) & 0x40)) {
    NRF24L01_ReadReg(NRF24L01_60_R_RX_PL_WID); // Packet ID 1311
    NRF24L01_ReadPayload(payload, 9); // Packet ID 1312
    NRF24L01_ReadReg(NRF24L01_17_FIFO_STATUS); // Packet ID 1313
    NRF24L01_FlushRx(); // Packet ID 1314
    NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x40); // Packet ID 1315
  }
}
