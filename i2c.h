/*
 * i2c.h
 *
 *  Created on: Apr 6, 2020
 *      Author: dcomes
 */

#ifndef LIBSTM32_I2C_H_
#define LIBSTM32_I2C_H_

#include "utility/pin.h"

namespace cmdc0de {

class I2CError : public ErrorType {
public:
	I2CError();
	I2CError(const uint32_t &en);
	~I2CError();
public:
	static const uint8_t NO_DEVICE_FOUND = 1;
};

class I2CDevice {
protected:
	I2CDevice(I2C_HandleTypeDef* p);
public:
	~I2CDevice();
	void deinit();
protected:
	I2C_HandleTypeDef* Port;
};

class I2CMaster : public I2CDevice {
public:
	enum ACK_TYPE {
		ACK_EVERY_BYTE = 0
		, NACK_EVERY_BYTE = 1
		, NACK_LAST_BYTE = 2
	};
	static const uint32_t DEFAULT_TIMEOUT=10;
public:
	I2CMaster(I2C_HandleTypeDef* p);
	~I2CMaster();
	bool init();
	void scan();
	I2CError probe(uint8_t address);
	bool write(uint8_t addr, uint8_t *data, uint16_t size);
	bool read (uint8_t addr, uint8_t *data, uint16_t size);
	bool write(uint8_t addr, uint8_t *data, uint16_t size, uint32_t timeout);
	bool read (uint8_t addr, uint8_t *data, uint16_t size, uint32_t timeout);
private:
};


}



#endif /* LIBSTM32_I2C_H_ */
