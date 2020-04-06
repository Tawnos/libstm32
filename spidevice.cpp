/*
 * spidevice.cpp
 *
 *  Created on: Apr 5, 2020
 *      Author: dcomes
 */



#include "config.h"
#include "spibus.h"
#include "spidevice.h"
#include "error_type.h"

using namespace cmdc0de;

//base spi device

SPIDevice::SPIDevice(SPIBus *b) : MyBus(b) {

}

bool SPIDevice::init()  const {
	return onInit();
}

ErrorType SPIDevice::shutdown()  const{
	return onShutdown();
}

ErrorType SPIDevice::sendAndReceive(uint8_t out, uint8_t &in)  const{
	return onSendAndReceive(out,in);
}

ErrorType SPIDevice::sendAndReceive(uint8_t *p, uint16_t len)  const{
	return onSendAndReceive(p,len);
}

ErrorType SPIDevice::sendAndReceive(uint8_t *out, uint8_t *in, uint16_t len)  const{
	return onSendAndReceive(out,in,len, nullptr);
}

ErrorType SPIDevice::sendAndReceive(uint8_t *out, uint8_t *in, uint16_t len, void *userData)  const{
	return onSendAndReceive(out,in,len, userData);
}

ErrorType SPIDevice::send(const uint8_t *p, uint16_t len)  const{
	return send(p,len, nullptr);
}

ErrorType SPIDevice::send(const uint8_t *p, uint16_t len, void *userData)  const{
	return onSend(p,len, userData);
}

SPIDevice::~SPIDevice() {

}


////////////////////////////////
//master
const char *SPIMaster::LOGTAG = "SPIMASTER";

ErrorType SPIMaster::onSendAndReceive(uint8_t out, uint8_t &in)  const{
	HAL_StatusTypeDef r;
	if(HAL_OK==(r=HAL_SPI_TransmitReceive(MyBus->getSPI(),&out,&in,sizeof(out),DEFAULT_TIMEOUT))) {
		return SPIError();
	}
	return SPIError(r);
}

ErrorType SPIMaster::onSendAndReceive(uint8_t *p, uint16_t len)  const{
	HAL_StatusTypeDef r;
	if(HAL_OK==(r=HAL_SPI_TransmitReceive(MyBus->getSPI(),p,p,len,DEFAULT_TIMEOUT))) {
		return SPIError();
	}
	return SPIError(r);
}

ErrorType SPIMaster::onSendAndReceive(uint8_t *out, uint8_t *in,uint16_t len, void *userData)  const{
	//TODO make interrupt driven
	HAL_StatusTypeDef r;
	if(HAL_OK==(r=HAL_SPI_TransmitReceive(MyBus->getSPI(),out,in,len,DEFAULT_TIMEOUT))) {
		return SPIError();
	}
	return SPIError(r);
}

ErrorType SPIMaster::onSend(const uint8_t *p, uint16_t len, void *userData) const {
	//TODO make interrupt driven
	HAL_StatusTypeDef r;
	if(HAL_OK==(r=HAL_SPI_Transmit(MyBus->getSPI(),(uint8_t*)p,len,DEFAULT_TIMEOUT))) {
		return SPIError();
	}
	return SPIError(r);
}

SPIMaster::~SPIMaster() {
	shutdown();
}

ErrorType SPIMaster::onShutdown() const {
	return getBus()->removeDevice(this);
}

SPIMaster::SPIMaster(SPIBus* s)	: SPIDevice(s) {

}

bool SPIMaster::onInit() const {
	return true;
}
