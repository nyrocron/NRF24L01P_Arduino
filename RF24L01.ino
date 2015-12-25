/*
 * Copyright (c) 2015 Florian Tautz 
 * Licensed under The MIT License, see LICENSE for licensing details.
 */

#include <SPI.h>
#include "RF24L01.h"

void rfInit()
{
  delay(5); // just to be safe

  regWrite(REG_CONFIG, 0b00001100);
  regWrite(REG_SETUP_RETR, 0b01011010);
  regWrite(REG_RF_SETUP, 0b00000000);
  //regWrite(REG_RF_SETUP, 0b00000110);
  regWrite(REG_FEATURE, 0b00000000);
  regWrite(REG_DYNPD, 0b00000000);
  regWrite(REG_STATUS, IRQ_MASK);
  regWrite(REG_RF_CH, 76);

  command(FLUSH_RX);
  command(FLUSH_TX);

  // power up and wait
  regWrite(REG_CONFIG, 0b00001110);
  delay(5);
}

bool rfSend(uint64_t addr, void *buf, int len)
{
  regWrite(REG_TX_ADDR, &addr, 5);
  regWrite(REG_RX_ADDR_P0, &addr, 5);
  
  command(W_TX_PAYLOAD, buf, len, PACKET_LEN);
  digitalWrite(cePin, HIGH);
  delayMicroseconds(10);
  digitalWrite(cePin, LOW);

  delay(PACKET_LEN);

  byte stat = regRead(REG_STATUS);
  regWrite(REG_STATUS, IRQ_MASK);
  
  if (stat & MAX_RT) {
    Serial.println("max retries reached");
    return false;
  }
  
  if (stat & TX_DS)
    return true;
  return false;
}

void command(byte cmd)
{
  beginTransaction();
  SPI.transfer(cmd);
  endTransaction();
}

void command(byte cmd, void *buf, int len)
{
  command(cmd, buf, len, len);
}

void command(byte cmd, void *buf, int len, int pad)
{
  beginTransaction();
  SPI.transfer(cmd);
  byte *data = (byte*)buf;
  for (int i = 0; i < len; i++)
    SPI.transfer(data[i]);
  for (int i = len; i < pad; i++)
    SPI.transfer(0);
  endTransaction();
}

void regWrite(byte addr, byte value)
{
  beginTransaction();
  SPI.transfer(W_REGISTER | addr);
  SPI.transfer(value);
  endTransaction();
}

void regWrite(byte addr, void *buf, int len)
{
  beginTransaction();
  SPI.transfer(W_REGISTER | addr);
  byte *data = (byte*)buf;
  for (int i = 0; i < len; i++)
    SPI.transfer(data[i]);
  endTransaction();
}

byte regRead(byte addr)
{
  beginTransaction();
  SPI.transfer(R_REGISTER | addr);
  byte value = SPI.transfer(0xff);
  endTransaction();
  return value;
}

void beginTransaction()
{
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  delayMicroseconds(10);
}

void endTransaction()
{
  delayMicroseconds(10);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
}
