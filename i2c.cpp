/*
 * i2c.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: dcomes
 */

#include "config.h"
#include "i2c.h"
#include <cstdio>

static const char tag[] = "I2C";
using namespace cmdc0de;


I2CError::I2CError() : ErrorType(Facility::ERROR_FACILITY_I2C, 0) { //, cmdc0de::ErrorType::NO_ERROR) {

}

I2CError::I2CError(const uint32_t &en) : ErrorType(Facility::ERROR_FACILITY_I2C,en) {

}

I2CError::~I2CError() {

}

///////////////////////////////////////

I2CDevice::I2CDevice(I2C_HandleTypeDef* p) : Port(p) {


}

void I2CDevice::deinit() {
	//msp_deinit()?
}

I2CDevice::~I2CDevice() {
	deinit();
}






////////////////////////////////////////////////////////////////////////////
I2CMaster::I2CMaster(I2C_HandleTypeDef* p ) : I2CDevice(p) {

}

I2CMaster::~I2CMaster() {

}

bool I2CMaster::init() {
	bool bRetVal = false;
	return bRetVal;
}

I2CError I2CMaster::probe(uint8_t address) {
	I2CError result;
	uint8_t wireAddress = address << 1u;
	if(HAL_OK != HAL_I2C_IsDeviceReady(Port,wireAddress,3u,10u)) {
		return I2CError(I2CError::NO_DEVICE_FOUND);
	}

	return result;
}

void I2CMaster::scan() {
	printf("Scanning I2C bus.");

	I2CError result;
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:   ");
	for (uint8_t address = 1; address < 0x78; address++) {
		result = probe(address);

		if (address % 16 == 0) {
			printf("\n%.2x:", address);
		}
		if (result.ok()) {
			printf(" %.2x", address);
		} else {
			printf(" --");
		}
	}
	printf("\n");
}

bool I2CMaster::write(uint8_t addr, uint8_t *data, uint16_t size) {
	return write(addr,data,size,DEFAULT_TIMEOUT);
}

bool I2CMaster::read(uint8_t addr, uint8_t *data, uint16_t size) {
	return read(addr,data,size,DEFAULT_TIMEOUT);
}

bool I2CMaster::write(uint8_t addr, uint8_t *data, uint16_t size, uint32_t timeout) {
	return HAL_OK==HAL_I2C_Master_Transmit(Port,addr,data,size,timeout);
}

bool I2CMaster::read(uint8_t addr, uint8_t *data, uint16_t size, uint32_t timeout) {
	return HAL_OK==HAL_I2C_Master_Transmit(Port,addr,data,size,timeout);
}



