/*
 * spidevice.h
 *
 *  Created on: Apr 5, 2020
 *      Author: dcomes
 */

#ifndef LIBSTM32_SPIDEVICE_H_
#define LIBSTM32_SPIDEVICE_H_

#include "error_type.h"

namespace cmdc0de {

class SPIBus;

class SPIDevice {
public:
	bool init() const;
	ErrorType shutdown() const;
	ErrorType sendAndReceive(uint8_t out, uint8_t &in) const;
	ErrorType sendAndReceive(uint8_t *p, uint16_t len) const;
	ErrorType sendAndReceive(uint8_t *out, uint8_t *in, uint16_t len) const;
	ErrorType sendAndReceive(uint8_t *out, uint8_t *in, uint16_t len, void *userData) const;
	ErrorType send(const uint8_t *p, uint16_t len) const;
	ErrorType send(const uint8_t *p, uint16_t len, void *userData) const;
	virtual ~SPIDevice();
	SPIBus *getBus() const {return MyBus;}
protected:
	SPIDevice(SPIBus* bus);
	virtual bool onInit() const=0;
	virtual ErrorType onShutdown() const=0;
	virtual ErrorType onSendAndReceive(uint8_t out, uint8_t &in) const=0;
	virtual ErrorType onSendAndReceive(uint8_t *p, uint16_t len) const=0;
	virtual ErrorType onSendAndReceive(uint8_t *out, uint8_t *in, uint16_t len, void *userData) const=0;
	virtual ErrorType onSend(const uint8_t *p, uint16_t len, void *userData) const=0;
protected:
	SPIBus *MyBus;
};

/////////
//
class SPIMaster : public SPIDevice {
public:
	static const uint32_t DEFAULT_TIMEOUT = 1000; //default communication timeout in ms
	virtual ~SPIMaster();
public:
	static const char *LOGTAG;
protected:
	ErrorType onShutdown() const;
	SPIMaster(SPIBus*b);
	virtual ErrorType onSendAndReceive(uint8_t out, uint8_t &in) const;
	virtual ErrorType onSendAndReceive(uint8_t *p, uint16_t len) const;
	virtual ErrorType onSendAndReceive(uint8_t *out, uint8_t *in, uint16_t len, void *userData) const;
	virtual ErrorType onSend(const uint8_t *p, uint16_t len, void *userData) const;
	virtual bool onInit() const;
private:
	friend class SPIBus;
};

}


#endif /* LIBSTM32_SPIDEVICE_H_ */
