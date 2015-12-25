/*
 * Copyright (c) 2015 Florian Tautz 
 * Licensed under The MIT License, see LICENSE for licensing details.
 */

#include <SPI.h>
#include "RF24L01.h"

const uint64_t txAddr = 0x0a0b0c0d0e;

void setup()
{
  // Serial setup
  Serial.begin(115200);
  
  // SPI setup
  pinMode(cePin, OUTPUT);
  pinMode(csPin, OUTPUT);
  SPI.begin();

  rfInit();
}

void loop()
{
  char *data = "Hello";
  if (rfSend(txAddr, data, 5))
    Serial.println("send ok");
  else
    Serial.println("send failed");

  delay(1000);
}
