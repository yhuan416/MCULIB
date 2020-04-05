#ifndef _SHT3X_H_
#define _SHT3X_H_
//-- Includes -----------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

//-- Defines ------------------------------------------------------------------
// System Little Endian
#define LITTLE_ENDIAN		(1)

// Sensor Address
#define SHT3X_ADDR_VSS		(0x44) /* default addr */
#define SHT3X_ADDR_VDD		(0x45)

//-- Enumerations -------------------------------------------------------------
// Error codes
typedef enum{
    NO_ERROR = 0x00,        // no error
    ACK_ERROR = 0x01,       // no acknowledgment error
    CHECKSUM_ERROR = 0x02,  // checksum mismatch error
    TIMEOUT_ERROR = 0x04,   // timeout error
    PARM_ERROR = 0x80,      // parameter out of range error
}etError;

// I2C acknowledge
typedef enum{
	NACK = 0,
	ACK = 1
}etI2cAck;

// Sensor Commands
typedef enum{
    CMD_READ_SERIALNBR 	= 0x3780, // read serial number
    CMD_READ_STATUS 	= 0xF32D, // read status register
    CMD_CLEAR_STATUS 	= 0x3041, // clear status register
    CMD_HEATER_ENABLE 	= 0x306D, // enabled heater
    CMD_HEATER_DISABLE 	= 0x3066, // disable heater
    CMD_SOFT_RESET 		= 0x30A2, // soft reset
    CMD_MEAS_CLOCKSTR_H = 0x2C06, // measurement: clock stretching, high repeatability
    CMD_MEAS_CLOCKSTR_M = 0x2C0D, // measurement: clock stretching, medium repeatability
    CMD_MEAS_CLOCKSTR_L = 0x2C10, // measurement: clock stretching, low repeatability
    CMD_MEAS_POLLING_H 	= 0x2400, // measurement: polling, high repeatability
    CMD_MEAS_POLLING_M 	= 0x240B, // measurement: polling, medium repeatability
    CMD_MEAS_POLLING_L 	= 0x2416, // measurement: polling, low repeatability
    CMD_MEAS_PERI_05_H 	= 0x2032, // measurement: periodic 0.5 mps, high repeatability
    CMD_MEAS_PERI_05_M 	= 0x2024, // measurement: periodic 0.5 mps, medium repeatability
    CMD_MEAS_PERI_05_L 	= 0x202F, // measurement: periodic 0.5 mps, low repeatability
    CMD_MEAS_PERI_1_H 	= 0x2130, // measurement: periodic 1 mps, high repeatability
    CMD_MEAS_PERI_1_M 	= 0x2126, // measurement: periodic 1 mps, medium repeatability
    CMD_MEAS_PERI_1_L 	= 0x212D, // measurement: periodic 1 mps, low repeatability
    CMD_MEAS_PERI_2_H 	= 0x2236, // measurement: periodic 2 mps, high repeatability
    CMD_MEAS_PERI_2_M 	= 0x2220, // measurement: periodic 2 mps, medium repeatability
    CMD_MEAS_PERI_2_L 	= 0x222B, // measurement: periodic 2 mps, low repeatability
    CMD_MEAS_PERI_4_H 	= 0x2334, // measurement: periodic 4 mps, high repeatability
    CMD_MEAS_PERI_4_M 	= 0x2322, // measurement: periodic 4 mps, medium repeatability
    CMD_MEAS_PERI_4_L 	= 0x2329, // measurement: periodic 4 mps, low repeatability
    CMD_MEAS_PERI_10_H 	= 0x2737, // measurement: periodic 10 mps, high repeatability
    CMD_MEAS_PERI_10_M 	= 0x2721, // measurement: periodic 10 mps, medium repeatability
    CMD_MEAS_PERI_10_L 	= 0x272A, // measurement: periodic 10 mps, low repeatability
    CMD_FETCH_DATA 		= 0xE000, // readout measurements for periodic mode
    CMD_R_AL_LIM_LS 	= 0xE102, // read alert limits, low set
    CMD_R_AL_LIM_LC 	= 0xE109, // read alert limits, low clear
    CMD_R_AL_LIM_HS 	= 0xE11F, // read alert limits, high set
    CMD_R_AL_LIM_HC 	= 0xE114, // read alert limits, high clear
    CMD_W_AL_LIM_HS 	= 0x611D, // write alert limits, high set
    CMD_W_AL_LIM_HC 	= 0x6116, // write alert limits, high clearSample Code for SHT3x
    CMD_W_AL_LIM_LC 	= 0x610B, // write alert limits, low clear
    CMD_W_AL_LIM_LS 	= 0x6100, // write alert limits, low set
    CMD_NO_SLEEP 		= 0x303E,
}etCommands;

// Measurement Repeatability
typedef enum{
    REPEATAB_HIGH, 	// high repeatability
    REPEATAB_MEDIUM, // medium repeatability
    REPEATAB_LOW, 	// low repeatability
}etRepeatability;

// Measurement Mode
typedef enum{
    MODE_CLKSTRETCH, // clock stretching
    MODE_POLLING, 	// polling
}etMode;

// Measurement Frequency
typedef enum{
    FREQUENCY_HZ5, 	// 0.5 measurements per seconds
    FREQUENCY_1HZ, 	// 1.0 measurements per seconds
    FREQUENCY_2HZ, 	// 2.0 measurements per seconds
    FREQUENCY_4HZ, 	// 4.0 measurements per seconds
    FREQUENCY_10HZ, // 10.0 measurements per seconds
}etFrequency;

//-- Typedefs -----------------------------------------------------------------
// Status-Register
typedef union {
    uint16_t value;
    struct{
        #if LITTLE_ENDIAN // bit-order is little endian
            uint16_t CrcStatus : 1; // write data checksum status
            uint16_t CmdStatus : 1; // command status
            uint16_t Reserve0 : 2; // reserved
            uint16_t ResetDetected : 1; // system reset detected
            uint16_t Reserve1 : 5; // reserved
            uint16_t T_Alert : 1; // temperature tracking alert
            uint16_t RH_Alert : 1; // humidity tracking alert
            uint16_t Reserve2 : 1; // reserved
            uint16_t HeaterStatus : 1; // heater status
            uint16_t Reserve3 : 1; // reserved
            uint16_t AlertPending : 1; // alert pending status
        #else // bit-order is big endian
            uint16_t AlertPending : 1;
            uint16_t Reserve3 : 1;
            uint16_t HeaterStatus : 1;
            uint16_t Reserve2 : 1;
            uint16_t RH_Alert : 1;
            uint16_t T_Alert : 1;
            uint16_t Reserve1 : 5;
            uint16_t ResetDetected : 1;
            uint16_t Reserve0 : 2;
            uint16_t CmdStatus : 1;
            uint16_t CrcStatus : 1;
        #endif
    }bit;
} regStatus;
//=============================================================================
// Initializes the I2C bus First for communication with the sensor.
//-----------------------------------------------------------------------------
// input: 
// i2cAddress : I2C address
//					SHT3X_ADDR_VSS 	(0x44 ADDR pin low)
//					SHT3X_ADDR_VDD	(0x45 ADDR pin high)
//-----------------------------------------------------------------------------
void SHT3X_Init(uint16_t i2cAddress);
//=============================================================================
// Sets the I2C address.
//-----------------------------------------------------------------------------
// input: 
// i2cAddress : I2C address
//					SHT3X_ADDR_VSS 	(0x44 ADDR pin low)
//					SHT3X_ADDR_VDD	(0x45 ADDR pin high)
//-----------------------------------------------------------------------------
void SHT3X_SetI2cAdr(uint16_t i2cAddress);
//=============================================================================
// Reads the serial number from sensor.
//-----------------------------------------------------------------------------
// input: 
// serialNumber : pointer to serialNumber
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3x_ReadSerialNumber(uint32_t* serialNumber);
//=============================================================================
// Reads the status register from the sensor.
//-----------------------------------------------------------------------------
// input: 
// status 	: pointer to status
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_ReadStatus(uint16_t* status);
//=============================================================================
// Clears all alert flags in status register from sensor.
//-----------------------------------------------------------------------------
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_ClearAllAlertFlags(void);
//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
//-----------------------------------------------------------------------------
// input: 
// temperature 		: pointer to temperature
// humiditiy 		: pointer to humidity
// repeatability 	: @etRepeatability, repeatability for the measurement [low, medium, high]
// mode 			: @etMode, command mode [clock stretching, polling]
// timeout 			: timeout in milliseconds
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatchSample Code for SHT3x
// 			TIMEOUT_ERROR 	= timeout
// 			PARM_ERROR 		= parameter out of range
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_GetTempAndHumi(float* temperature, 
							 float* humiditiy,
							 etRepeatability repeatability, 
							 etMode mode,
							 uint8_t timeout);
//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
// This function uses the i2c clock stretching for waiting until measurement is
// ready.
//-----------------------------------------------------------------------------
// input: 
// temperature 		: pointer to temperature
// humiditiy 		: pointer to humidity
// repeatability 	: @etRepeatability, repeatability for the measurement [low, medium, high]
// timeout 			: clock stretching timeout in milliseconds
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			PARM_ERROR 		= parameter out of range
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_GetTempAndHumiClkStretch(float* temperature, 
									   float* humiditiy,
									   etRepeatability repeatability,
									   uint8_t timeout);
//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
// This function polls every 1ms until measurement is ready.
//-----------------------------------------------------------------------------
// input: 
// temperature 		: pointer to temperature
// humiditiy 		: pointer to humidity
// repeatability 	: @etRepeatability, repeatability for the measurement [low, medium, high]
// timeout 			: polling timeout in milliseconds
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			PARM_ERROR 		= parameter out of range
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_GetTempAndHumiPolling(float* temperature, 
									float* humiditiy,
									etRepeatability repeatability,
									uint8_t timeout);
//=============================================================================
// Starts periodic measurement.
//-----------------------------------------------------------------------------
// input: 
// repeatability 	: @etRepeatability, repeatability for the measurement [low, medium, high]
// frequency 		: @etFrequency, measurement frequency [0.5, 1, 2, 4, 10] Hz
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			PARM_ERROR 		= parameter out of range
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_StartPeriodicMeasurment(etRepeatability repeatability, 
									  etFrequency frequency);
//=============================================================================
// Reads last measurement from the sensor buffer
//-----------------------------------------------------------------------------
// input: 
// temperature 	: pointer to temperature
// humidity 	: pointer to humidity
//
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_ReadMeasurementBuffer(float* temperature, float* humidity);
//=============================================================================
// Enables the heater on sensor
//-----------------------------------------------------------------------------
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_EnableHeater(void);
//=============================================================================
// Disables the heater on sensor
//-----------------------------------------------------------------------------
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_DisableHeater(void);
//=============================================================================
//
//-----------------------------------------------------------------------------
etError SHT3X_SetAlertLimits(float humidityHighSet, 
							 float temperatureHighSet,
							 float humidityHighClear, 
							 float temperatureHighClear,
							 float humidityLowClear, 
							 float temperatureLowClear,
							 float humidityLowSet, 
							 float temperatureLowSet);
//=============================================================================
//
//-----------------------------------------------------------------------------
etError SHT3X_GetAlertLimits(float* humidityHighSet, 
							 float* temperatureHighSet,
							 float* humidityHighClear, 
							 float* temperatureHighClear,
							 float* humidityLowClear, 
							 float* temperatureLowClear,
							 float* humidityLowSet, 
							 float* temperatureLowSet);
//=============================================================================
// Returns the state of the Alert-Pin.
//-----------------------------------------------------------------------------
// return: 
//		true: 	Alert-Pin is high
// 		false: 	Alter-Pin is low
//-----------------------------------------------------------------------------
bool SHT3X_ReadAlert(void);
//=============================================================================
// Calls the soft reset mechanism that forces the sensor into a well-defined
// state without removing the power supply.
//-----------------------------------------------------------------------------
// return: 
// error: 	ACK_ERROR 		= no acknowledgment from sensor
// 			CHECKSUM_ERROR 	= checksum mismatch
// 			TIMEOUT_ERROR 	= timeout
// 			NO_ERROR 		= no error
//-----------------------------------------------------------------------------
etError SHT3X_SoftReset(void);
//=============================================================================
// Resets the sensor by pulling down the reset pin.
//-----------------------------------------------------------------------------
void SHT3X_HardReset(void);

#endif

