/*
 * bmp280.h
 *
 *  Created on: Oct 7, 2017
 *      Author: cmdc0de
 */

#ifndef LIBSTM32_SENSORS_BMP280_BMP280_H_
#define LIBSTM32_SENSORS_BMP280_BMP280_H_

#include "../../error_type.h"
#include "../../config.h"

namespace cmdc0de {

/*
 * @author: cmdc0de
 * Warning. Holding any interface pin (SDI, SDO, SCK or CSB) at a logical high level when VDDIO is
 * switched off can permanently damage the device due caused by excessive current flow through
 * the ESD protection diodes.
 */
class BMP280 {
public:
	//error enums
	enum BMP280ErrorEnum {
		ERROR_INVALID_DEVICE = 1
	};
public:
	static const int BMP280_I2C_ADDRESS = 0x77;

	static const int BMP280_DIG_T1_LSB_REG = 0x88;
	static const int BMP280_DIG_T1_MSB_REG = 0x89;
	static const int BMP280_DIG_T2_LSB_REG = 0x8A;
	static const int BMP280_DIG_T2_MSB_REG = 0x8B;
	static const int BMP280_DIG_T3_LSB_REG = 0x8C;
	static const int BMP280_DIG_T3_MSB_REG = 0x8D;
	static const int BMP280_DIG_P1_LSB_REG = 0x8E;
	static const int BMP280_DIG_P1_MSB_REG = 0x8F;
	static const int BMP280_DIG_P2_LSB_REG = 0x90;
	static const int BMP280_DIG_P2_MSB_REG = 0x91;
	static const int BMP280_DIG_P3_LSB_REG = 0x92;
	static const int BMP280_DIG_P3_MSB_REG = 0x93;
	static const int BMP280_DIG_P4_LSB_REG = 0x94;
	static const int BMP280_DIG_P4_MSB_REG = 0x95;
	static const int BMP280_DIG_P5_LSB_REG = 0x96;
	static const int BMP280_DIG_P5_MSB_REG = 0x97;
	static const int BMP280_DIG_P6_LSB_REG = 0x98;
	static const int BMP280_DIG_P6_MSB_REG = 0x99;
	static const int BMP280_DIG_P7_LSB_REG = 0x9A;
	static const int BMP280_DIG_P7_MSB_REG = 0x9B;
	static const int BMP280_DIG_P8_LSB_REG = 0x9C;
	static const int BMP280_DIG_P8_MSB_REG = 0x9D;
	static const int BMP280_DIG_P9_LSB_REG = 0x9E;
	static const int BMP280_DIG_P9_MSB_REG = 0x9F;
	static const int BMP280_DIG_H1_REG = 0xA1;
	//The “id” register contains the chip identification number chip_id[7:0], which is 0x58. This number
	//can be read as soon as the device finished the power-on-reset.
	static const int BMP280_CHIP_ID_REG = 0xD0; //Chip ID
	static const int BMP280_VALID_CHIP_ID = 0x58; //BME280 MAGIC 0x60
	//The “reset” register contains the soft reset word reset[7:0]. If the value 0xB6 is written to the
	//register, the device is reset using the complete power-on-reset procedure. Writing other values
	//than 0xB6 has no effect. The readout value is always 0x00
	static const int BMP280_RST_REG = 0xE0; //Softreset Reg
	static const int BMP280_RST_MAGIC = 0xB6;
	//
	static const int BMP280_DIG_H2_LSB_REG = 0xE1;
	static const int BMP280_DIG_H2_MSB_REG = 0xE2;
	static const int BMP280_DIG_H3_REG = 0xE3;
	static const int BMP280_DIG_H4_MSB_REG = 0xE4;
	static const int BMP280_DIG_H4_LSB_REG = 0xE5;
	static const int BMP280_DIG_H5_MSB_REG = 0xE6;
	static const int BMP280_DIG_H6_REG = 0xE7;
	static const int BMP280_CTRL_HUMIDITY_REG = 0xF2; //Ctrl Humidity Reg
	////////////////////////////////////////////////////
	//The “status” register contains two bits which indicate the status of the device
	//bit 3:  Automatically set to ‘1’ whenever a conversion is running and back to ‘0’ when the results have been transferred
	//			to the data registers.
	//bit 0: Automatically set to ‘1’ when the NVM data are being copied to image registers and back to ‘0’ when the
	//		copying is done. The data are copied at power-on-reset and before every conversion
	static const int BMP280_STAT_REG = 0xF3; //Status Reg
	static const int BMP280_CONVERSION_RUNNING_BIT_MASK = 0x8;
	static const int BMP280_NVM_DATA_BEING_COPIED_MASK = 0x1;
	/////////////////////////////////
	// The “ctrl_meas” register sets the data acquisition options of the device
	// Bit 1,0: Controls the power mode of the device. See chapter 3.6 for details
	// bit 4,3,2: Controls oversampling of pressure data
	// bit 7,6,5: Controls oversampling of temperature data
	static const int BMP280_CTRL_MEAS_REG = 0xF4; //Ctrl Measure Reg
	enum PRESSURE_SAMPLING {
		OVERSAMPLING_OFF = 0x0
		, OVERSAMPLING_1X = 0x1
		, OVERSAMPLING_2X = 0x2
		, OVERSAMPLING_4X = 0x3
		, OVERSAMPLING_8x = 0x4
		, OVERSAMPLING_16x = 0x5
	};
	enum TEMP_SAMPLING {
		T_OVERSAMPLING_OFF = 0x0
		, T_OVERSAMPLING_1X = 0x1
		, T_OVERSAMPLING_2X = 0x2
		, T_OVERSAMPLING_4X = 0x3
		, T_OVERSAMPLING_8x = 0x4
		, T_OVERSAMPLING_16x = 0x5
	};
	enum POWER_MODE {
		SLEEP_MODE = 0x0 //In sleep mode, no measurements are performed and power consumption (IDDSM) is at a minimum
		,
		FORCED_MODE = 0x1
		,
		NORMAL_MODE = 0x3 //Normal mode continuously cycles between an (active) measurement period and an (inactive)
						  //standby period, whose time is defined by tstandby
	};
	////////////////////////////////
	// when in normal mode, we continuously cycle
	static const int BMP280_CONFIG_REG = 0xF5; //Configuration Reg
	//bit 7,6,5 controls inactive duration tstanby
	// bit 4,3,2 controls the time constant of the IIR filter
	//Bit 0: enables 3 wire SPI when set to 1:  // we set this to 0 on startup
	enum NORMAL_MODE_CYCLE_TIME {
		HALF_MS = 0x0
		, MS_62_5 = 0x1
		, MS_125 = 0x2
		, MS_250 = 0x3
		, MS_500 = 0x4
		, MS_1000 = 0x5
		, MS_2000 = 0x6
		, MS_4000 = 0x7
	};
	enum FILTER_COEFFICIENT {
		FILTER_OFF = 0
		, COEFFICIENT_2 = 2
		, COEFFICIENT_4 = 4
		, COEFFICIENT_8 = 8
		, COEFFICIENT_16 = 16
	};
	///////////////////////////
	static const int BMP280_PRESSURE_MSB_REG = 0xF7; //Pressure MSB
	static const int BMP280_PRESSURE_LSB_REG = 0xF8; //Pressure LSB
	static const int BMP280_PRESSURE_XLSB_REG = 0xF9; //Pressure XLSB
	static const int BMP280_TEMPERATURE_MSB_REG = 0xFA; //Temperature MSB
	static const int BMP280_TEMPERATURE_LSB_REG = 0xFB; //Temperature LSB
	static const int BMP280_TEMPERATURE_XLSB_REG = 0xFC; //Temperature XLSB
	static const int BMP280_HUMIDITY_MSB_REG = 0xFD; //Humidity MSB
	static const int BMP280_HUMIDITY_LSB_REG = 0xFE; //Humidity LSB
public:
	class Hardware {
	public:
		static I2C_HandleTypeDef *getI2C(uint8_t i2caddress);
	};
	public:

	BMP280();
	ErrorType init(const PRESSURE_SAMPLING &ps, const TEMP_SAMPLING &ts, const POWER_MODE &pm,
			const NORMAL_MODE_CYCLE_TIME &ct, const FILTER_COEFFICIENT &fc);

	//Software reset routine
	void reset(void);

	//Returns the values as floats.
	float readFloatPressure(void);
	float readFloatAltitudeMeters(void);
	float readFloatAltitudeFeet(void);
	float readFloatHumidity(void);

	//Temperature related methods
	float readTempC(void);
	float readTempF(void);

	//The following utilities read and write

	//readRegister reads one register
	uint8_t readRegister(uint8_t);
	//Writes a byte;
	void writeRegister(uint8_t, uint8_t);
	void setPowerMode(const POWER_MODE &pm);
	void setSampling(const PRESSURE_SAMPLING &ps, const TEMP_SAMPLING &ts, const POWER_MODE &pm);
	void setTimeStampbyAndFilter(const NORMAL_MODE_CYCLE_TIME &ct, const FILTER_COEFFICIENT &fc);
protected:
	// Hold Calibration Data
	struct SensorCalibration {
	public:
		uint16_t dig_T1;
		int16_t dig_T2;
		int16_t dig_T3;
		uint16_t dig_P1;
		int16_t dig_P2;
		int16_t dig_P3;
		int16_t dig_P4;
		int16_t dig_P5;
		int16_t dig_P6;
		int16_t dig_P7;
		int16_t dig_P8;
		int16_t dig_P9;
		uint8_t dig_H1;
		int16_t dig_H2;
		uint8_t dig_H3;
		int16_t dig_H4;
		int16_t dig_H5;
		uint8_t dig_H6;
	};
private:
	uint8_t I2CAddress;
	SensorCalibration Calibration;
	int32_t TFine;
};

} /* namespace cmdc0de */

#endif /* LIBSTM32_SENSORS_BMP280_BMP280_H_ */
