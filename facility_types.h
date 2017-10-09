/*
 * facility_types.h
 *
 *  Created on: Oct 8, 2017
 *      Author: dcomes
 */

#ifndef LIBSTM32_FACILITY_TYPES_H_
#define LIBSTM32_FACILITY_TYPES_H_

enum Facility {
	ERROR_FACILITY_GENERAL = 0
	, ERROR_FACILITY_SENSOR_BMP280 = 1
	, ERROR_FACILITY_APPLICATION = 255
};

#endif /* LIBSTM32_FACILITY_TYPES_H_ */
