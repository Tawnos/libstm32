/*
 * apa102c.cpp
 *
 *  Created on: Apr 4, 2020
 *      Author: dcomes
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apa102c.h"
#include "../logger.h"
#include "../spidevice.h"
#include "../rgbcolor.h"

using namespace cmdc0de;



const char *APA102c::LOG = "APA102c";

APA102c::APA102c(const SPIDevice *spiI) : SPIInterface(spiI), BufferSize(0), LedBuffer1(0) {}

void APA102c::init(uint16_t nleds, RGB *ledBuf) {
	delete [] LedBuffer1;
	BufferSize = (nleds*4)+4;
	LedBuffer1 = new char [BufferSize];
	int bufOff = 0;
	LedBuffer1[bufOff]   = 0x0;
	LedBuffer1[++bufOff] = 0x0;
	LedBuffer1[++bufOff] = 0x0;
	LedBuffer1[++bufOff] = 0x0;
	for(int l=0;l<nleds;++l) {
		uint8_t bright = ledBuf[l].getBrightness();
		bright = (uint8_t)(((float)bright/100.0f)*MAX_BRIGHTNESS);
		LedBuffer1[++bufOff] = BRIGHTNESS_START_BITS|bright;
		LedBuffer1[++bufOff] = ledBuf[l].getBlue();
		LedBuffer1[++bufOff] = ledBuf[l].getGreen();
		LedBuffer1[++bufOff] = ledBuf[l].getRed();
	}
	//LedBuffer1[++bufOff] = 0xFF;
	//LedBuffer1[++bufOff] = 0xFF;
	//LedBuffer1[++bufOff] = 0xFF;
	//LedBuffer1[++bufOff] = 0xFF;
}

void APA102c::setBrightness(uint8_t t) {
	uint8_t bright = t;
	bright = (uint8_t)(((float)bright/100.0f)*MAX_BRIGHTNESS);
	for(int i=4;i<BufferSize;i+=4) {
		LedBuffer1[i]=BRIGHTNESS_START_BITS|bright;
	}
}


void APA102c::send() {
	if(BufferSize>0) {
		DBGMSG(static_cast<const char *>("%s: sending %d leds r[0]:%d, g[0]:%d, b[0]:%d, B[0]:%d\n"), APA102c::LOG, (BufferSize/4)-8,
			LedBuffer1[3], LedBuffer1[2], LedBuffer1[1], LedBuffer1[0]);
		//ESP_LOG_BUFFER_HEX(APA102c::LOG, LedBuffer1, BufferSize);
		SPIInterface->send((uint8_t*)LedBuffer1,BufferSize);
	}
}

void APA102c::send(uint16_t nleds, RGB *ledBuf) {
	init(nleds,ledBuf);
	send();
}
