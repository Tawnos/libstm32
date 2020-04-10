/*
 * spibus.h
 *
 *  Created on: Apr 5, 2020
 *      Author: dcomes
 */

#ifndef LIBSTM32_CMDC0DE_SPIBUS_H_
#define LIBSTM32_CMDC0DE_SPIBUS_H_

#include "config.h"
#include "error_type.h"
#include <etl/list.h>

namespace cmdc0de {

class SPIDevice;

#ifndef MAX_SPI_DEVICES
#define MAX_SPI_DEVICES 3
#endif

class SPIError : public ErrorType {
public:
	SPIError();
	SPIError(const uint32_t &en);
	~SPIError();
};

class SPIBus {
public:
	SPIBus(SPI_HandleTypeDef *spi) : hspi(spi){}
	SPI_HandleTypeDef *getSPI() {return hspi;}
	/*
	* create a master device for this SPI Bus
	*/
	const SPIDevice *createMasterDevice();
	/*
	* removes the device from this Bus, SPI device itself is not destroyed, device is not destoryed
	*/
	ErrorType removeDevice(const SPIDevice *d);
	/*
	* remove all devices, destroy all devices, free spi bus
	*/
	ErrorType shutdown();
private:
	SPI_HandleTypeDef *hspi;
	etl::list<SPIDevice *,MAX_SPI_DEVICES> Devicelist;
};

}

#endif /* LIBSTM32_SPIBUS_H_ */
