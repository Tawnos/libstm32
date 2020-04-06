/*
 * spibus.cpp
 *
 *  Created on: Apr 5, 2020
 *      Author: dcomes
 */

#include "config.h"
#include "spibus.h"
#include "spidevice.h"

using namespace cmdc0de;

SPIError::SPIError() : ErrorType(Facility::ERROR_FACILITY_SPI, 0) { //, cmdc0de::ErrorType::NO_ERROR) {

}

SPIError::SPIError(const uint32_t &en) : ErrorType(Facility::ERROR_FACILITY_SPI,en) {

}

SPIError::~SPIError() {

}



////////////////////////////////////
const SPIDevice *SPIBus::createMasterDevice() {
	SPIDevice *p = new SPIMaster(this);
	Devicelist.push_back(p);
	return p;
}

ErrorType SPIBus::removeDevice(const SPIDevice *d) {
	//TODO
	return SPIError();
}

ErrorType SPIBus::shutdown() {
	return SPIError();
}
