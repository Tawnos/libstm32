/*
 * bmp280.cpp
 *
 *  Created on: Oct 7, 2017
 *      Author: dcomes
 */

#include "bmp280.h"
#include <math.h>

using cmdc0de::BMP280;
using cmdc0de::ErrorType;

//****************************************************************************//
//
//  Settings and configuration
//
//****************************************************************************//

//Constructor -- Specifies default configuration
BMP280::BMP280() : I2CAddress(BMP280::BMP280_I2C_ADDRESS), Calibration(), TFine(0) {
}


void BMP280::setSampling(const PRESSURE_SAMPLING &ps, const TEMP_SAMPLING &ts, const POWER_MODE &pm) {
	uint8_t samplingValue = (ps << 2) | (ts << 5) | pm;
	writeRegister(BMP280_CTRL_MEAS_REG, samplingValue);
}

void BMP280::setTimeStampbyAndFilter(const NORMAL_MODE_CYCLE_TIME &ct, const FILTER_COEFFICIENT &fc) {
	uint8_t value = (ct<<5)|(fc<<2)|0;
	writeRegister(BMP280_CONFIG_REG,value);
}

void BMP280::setPowerMode(const POWER_MODE &pm) {
	setSampling(PRESSURE_SAMPLING::OVERSAMPLING_OFF, TEMP_SAMPLING::T_OVERSAMPLING_OFF, pm);
}


ErrorType BMP280::init(const PRESSURE_SAMPLING &ps, const TEMP_SAMPLING &ts, const POWER_MODE &pm,
		const NORMAL_MODE_CYCLE_TIME &ct, const FILTER_COEFFICIENT &fc) {

	uint8_t chipID = readRegister(BMP280_CHIP_ID_REG);
	if(chipID!=BMP280_VALID_CHIP_ID) {
		return ErrorType(ERROR_FACILITY_SENSOR_BMP280,BMP280::ERROR_INVALID_DEVICE);
	}
	//Reading all compensation data, range 0x88:A1, 0xE1:E7
	Calibration.dig_T1 =
			((uint16_t) ((readRegister(BMP280_DIG_T1_MSB_REG) << 8) + readRegister(BMP280_DIG_T1_LSB_REG)));
	Calibration.dig_T2 = ((int16_t) ((readRegister(BMP280_DIG_T2_MSB_REG) << 8) + readRegister(BMP280_DIG_T2_LSB_REG)));
	Calibration.dig_T3 = ((int16_t) ((readRegister(BMP280_DIG_T3_MSB_REG) << 8) + readRegister(BMP280_DIG_T3_LSB_REG)));

	Calibration.dig_P1 =
			((uint16_t) ((readRegister(BMP280_DIG_P1_MSB_REG) << 8) + readRegister(BMP280_DIG_P1_LSB_REG)));
	Calibration.dig_P2 = ((int16_t) ((readRegister(BMP280_DIG_P2_MSB_REG) << 8) + readRegister(BMP280_DIG_P2_LSB_REG)));
	Calibration.dig_P3 = ((int16_t) ((readRegister(BMP280_DIG_P3_MSB_REG) << 8) + readRegister(BMP280_DIG_P3_LSB_REG)));
	Calibration.dig_P4 = ((int16_t) ((readRegister(BMP280_DIG_P4_MSB_REG) << 8) + readRegister(BMP280_DIG_P4_LSB_REG)));
	Calibration.dig_P5 = ((int16_t) ((readRegister(BMP280_DIG_P5_MSB_REG) << 8) + readRegister(BMP280_DIG_P5_LSB_REG)));
	Calibration.dig_P6 = ((int16_t) ((readRegister(BMP280_DIG_P6_MSB_REG) << 8) + readRegister(BMP280_DIG_P6_LSB_REG)));
	Calibration.dig_P7 = ((int16_t) ((readRegister(BMP280_DIG_P7_MSB_REG) << 8) + readRegister(BMP280_DIG_P7_LSB_REG)));
	Calibration.dig_P8 = ((int16_t) ((readRegister(BMP280_DIG_P8_MSB_REG) << 8) + readRegister(BMP280_DIG_P8_LSB_REG)));
	Calibration.dig_P9 = ((int16_t) ((readRegister(BMP280_DIG_P9_MSB_REG) << 8) + readRegister(BMP280_DIG_P9_LSB_REG)));

	Calibration.dig_H1 = ((uint8_t) (readRegister(BMP280_DIG_H1_REG)));
	Calibration.dig_H2 = ((int16_t) ((readRegister(BMP280_DIG_H2_MSB_REG) << 8) + readRegister(BMP280_DIG_H2_LSB_REG)));
	Calibration.dig_H3 = ((uint8_t) (readRegister(BMP280_DIG_H3_REG)));
	Calibration.dig_H4 = ((int16_t) ((readRegister(BMP280_DIG_H4_MSB_REG) << 4)
			+ (readRegister(BMP280_DIG_H4_LSB_REG) & 0x0F)));
	Calibration.dig_H5 = ((int16_t) ((readRegister(BMP280_DIG_H5_MSB_REG) << 4)
			+ ((readRegister(BMP280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
	Calibration.dig_H6 = ((uint8_t) readRegister(BMP280_DIG_H6_REG));

	//config will only be writeable in sleep mode, so first insure that.
	setPowerMode(POWER_MODE::SLEEP_MODE);

	//now set sampling
	setTimeStampbyAndFilter(ct,fc);

	//Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	//dataToWrite = settings.humidOverSample & 0x07; //all other bits can be ignored
	//writeRegister(BMP280_CTRL_HUMIDITY_REG, dataToWrite);

	setSampling(ps,ts,pm);

	return ErrorType();
}


void BMP280::reset(void) {
	writeRegister(BMP280_RST_REG, BMP280_RST_MAGIC);
}

//****************************************************************************//
//
//  Pressure Section
//
//****************************************************************************//
float BMP280::readFloatPressure(void) {

	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
	int32_t adc_P = ((uint32_t) readRegister(BMP280_PRESSURE_MSB_REG) << 12)
			| ((uint32_t) readRegister(BMP280_PRESSURE_LSB_REG) << 4)
			| ((readRegister(BMP280_PRESSURE_XLSB_REG) >> 4) & 0x0F);

	int64_t var1, var2, p_acc;
	var1 = ((int64_t) TFine) - 128000;
	var2 = var1 * var1 * (int64_t) Calibration.dig_P6;
	var2 = var2 + ((var1 * (int64_t) Calibration.dig_P5) << 17);
	var2 = var2 + (((int64_t) Calibration.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) Calibration.dig_P3) >> 8) + ((var1 * (int64_t) Calibration.dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) Calibration.dig_P1) >> 33;
	if (var1 == 0)
			{
		return 0; // avoid exception caused by division by zero
	}
	p_acc = 1048576 - adc_P;
	p_acc = (((p_acc << 31) - var2) * 3125) / var1;
	var1 = (((int64_t) Calibration.dig_P9) * (p_acc >> 13) * (p_acc >> 13)) >> 25;
	var2 = (((int64_t) Calibration.dig_P8) * p_acc) >> 19;
	p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t) Calibration.dig_P7) << 4);

	return (float) p_acc / 256.0;

}

float BMP280::readFloatAltitudeMeters(void) {
	float heightOutput = 0;
	heightOutput = ((float) -45846.2) * (pow(((float) readFloatPressure() / (float) 101325), 0.190263) - (float) 1);
	return heightOutput;
}

float BMP280::readFloatAltitudeFeet(void) {
	float heightOutput = 0;
	heightOutput = readFloatAltitudeMeters() * 3.28084;
	return heightOutput;
}

float BMP280::readFloatHumidity(void) {
	// Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
	int32_t adc_H = ((uint32_t) readRegister(BMP280_HUMIDITY_MSB_REG) << 8)
			| ((uint32_t) readRegister(BMP280_HUMIDITY_LSB_REG));

	int32_t var1;
	var1 = (TFine - ((int32_t) 76800));
	var1 = (((((adc_H << 14) - (((int32_t) Calibration.dig_H4) << 20) - (((int32_t) Calibration.dig_H5) * var1)) +
			((int32_t) 16384)) >> 15)
			* (((((((var1 * ((int32_t) Calibration.dig_H6)) >> 10)
					* (((var1 * ((int32_t) Calibration.dig_H3)) >> 11) + ((int32_t) 32768))) >> 10)
					+ ((int32_t) 2097152)) *
					((int32_t) Calibration.dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t) Calibration.dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);
	return (float) (var1 >> 12) / 1024.0;
}

//****************************************************************************//
//
//  Temperature Section
//
//****************************************************************************//
float BMP280::readTempC(void) {
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value

	//get the reading (adc_T);
	int32_t adc_T = ((uint32_t) readRegister(BMP280_TEMPERATURE_MSB_REG) << 12)
			| ((uint32_t) readRegister(BMP280_TEMPERATURE_LSB_REG) << 4)
			| ((readRegister(BMP280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);

	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T >> 3) - ((int32_t) Calibration.dig_T1 << 1))) * ((int32_t) Calibration.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t) Calibration.dig_T1)) * ((adc_T >> 4) - ((int32_t) Calibration.dig_T1))) >> 12)
			*
			((int32_t) Calibration.dig_T3)) >> 14;
	TFine = var1 + var2;
	float output = (TFine * 5 + 128) >> 8;

	output = output / 100;

	return output;
}

float BMP280::readTempF(void) {
	float output = readTempC();
	output = (output * 9) / 5 + 32;
	return output;
}

uint8_t BMP280::readRegister(uint8_t offset) {
	//Return value
	HAL_I2C_Master_Transmit(Hardware::getI2C(I2CAddress),I2CAddress,&offset,1,100);
	HAL_I2C_Master_Receive(Hardware::getI2C(I2CAddress),I2CAddress,&offset,1,100);
	return offset;
}


void BMP280::writeRegister(uint8_t offset, uint8_t dataToWrite) {
	uint8_t data[2];
	data[0] = offset;
	data[1] = dataToWrite;
	HAL_I2C_Master_Transmit(Hardware::getI2C(I2CAddress),I2CAddress,&data[0],2,100);
}

