/*==================================================================================================
 * Project : RTD AUTOSAR 4.9
 * Platform : CORTEXM
 * Peripheral : S32K3XX
 * Dependencies : none
 *
 * Autosar Version : 4.9.0
 * Autosar Revision : ASR_REL_4_9_REV_0000
 * Autosar Conf.Variant :
 * SW Version : 7.0.1
 * Build Version : S32K3_RTD_7_0_1_D2602_ASR_REL_4_9_REV_0000_20260206
 *
 * Copyright 2020 - 2026 NXP
 *
 *   NXP Proprietary. This software is owned or controlled by NXP and may only be
 *   used strictly in accordance with the applicable license terms. By expressly
 *   accepting such terms or by downloading, installing, activating and/or otherwise
 *   using the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to be
 *   bound by the applicable license terms, then you may not retain, install,
 *   activate or otherwise use the software.
 ==================================================================================================*/

/**
 *   @file main.c
 *
 *   @brief   Main application file for Servo Control via ADC
 *   @details This application reads an ADC value from a potentiometer and
 *            controls a servo motor position through the PCA9685 PWM controller.
 *
 *   @addtogroup main_module main module documentation
 *   @{
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
 *                                        INCLUDE FILES
 * 1) system and project includes
 * 2) needed interfaces from external units
 * 3) internal and external interfaces from this unit
 ==================================================================================================*/
#include "Mcal.h"
#include "Clock_Ip.h"
#include "Siul2_Port_Ip.h"
#include "Siul2_Dio_Ip.h"
#include "IntCtrl_Ip.h"
#include "Adc_Sar_Ip.h"
#include "Lpi2c_Ip.h"
#include "OsIf.h"
#include "PCA9685.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 ==================================================================================================*/

/*==================================================================================================
 *                                      DEFINES AND MACROS
 ==================================================================================================*/

/*-------------------------------------------------------------------------------------------------
 * ADC Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief ADC hardware unit instance */
#define ADC_INSTANCE            ADCHWUNIT_0_INSTANCE
#define USE_POT_CLICK           1U

/** @brief ADC channel */
#define ADC_CHANNEL         1U

/** @brief ADC resolution */
#define ADC_RESOLUTION      	1<<12

/*-------------------------------------------------------------------------------------------------
 * Servo Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief Servo PWM channel on PCA9685 (0-15) */
#define SERVO_CHANNEL           0U

/*-------------------------------------------------------------------------------------------------
 * Main Loop Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief Delay between servo position updates in milliseconds */
#define MAIN_LOOP_DELAY_MS      20U

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 ==================================================================================================*/

/*==================================================================================================
 *                                      LOCAL VARIABLES
 ==================================================================================================*/
/** @brief Flag indicating ADC end-of-chain notification triggered */
static volatile boolean notif_triggered = FALSE;

/** @brief Raw ADC conversion data */
static volatile uint16 adcRawData = 0U;

/** @brief Max possible servo movement */
static uint16 adcMaxValue = ADC_RESOLUTION;

/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 ==================================================================================================*/

/*==================================================================================================
 *                                      GLOBAL VARIABLES
 ==================================================================================================*/

/*==================================================================================================
 *                                   EXTERNAL FUNCTION PROTOTYPES
 ==================================================================================================*/
/** @brief ADC SAR instance 0 interrupt service routine */
extern void Adc_Sar_0_Isr(void);

/*==================================================================================================
 *                                   LOCAL FUNCTION PROTOTYPES
 ==================================================================================================*/
static void Delay_ms(uint32 ms);
static uint16 ReadAdc(void);

/*==================================================================================================
 *                                       LOCAL FUNCTIONS
 ==================================================================================================*/

/**
 * @brief       Software delay in milliseconds
 * @details     Implements a busy-wait delay using NOP instructions.
 *              Calibrated for S32K3 at default clock frequency.
 *
 * @param[in]   ms  Number of milliseconds to delay
 *
 * @return      void
 */
static void Delay_ms(uint32 ms) {
	volatile uint32 i, j;
	for (i = 0U; i < ms; i++) {
		for (j = 0U; j < 16000U; j++) {
			__asm volatile ("nop");
		}
	}
}

/**
 * @brief       Read ADC conversion value
 * @details     Starts an ADC conversion and waits for the end-of-chain
 *              notification before returning the converted value.
 *
 * @return      uint16  ADC conversion result (0 to ADC_RESOLUTION)
 */
static uint16 ReadAdc(void) {
	Adc_Sar_Ip_StartConversion(ADC_INSTANCE, ADC_SAR_IP_CONV_CHAIN_NORMAL);
	while (!notif_triggered) {
		/* Wait for conversion complete */
	}
	notif_triggered = FALSE;
	return adcRawData;
}

/*==================================================================================================
 *                                   CALLBACK FUNCTIONS
 ==================================================================================================*/

/**
 * @brief       ADC End-of-Chain Notification Callback
 * @details     Called when ADC conversion chain completes. Stores the
 *              converted value and sets the notification flag.
 *
 * @return      void
 */
void AdcEndOfChainNotif(void) {
	adcRawData = Adc_Sar_Ip_GetConvData(ADC_INSTANCE, ADC_CHANNEL);
	notif_triggered = TRUE;
}

/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 ==================================================================================================*/

/**
 * @brief       Main application entry point
 * @details     Initializes all peripherals (Clock, Port, I2C, ADC) and the
 *              PCA9685 PWM controller. Runs a servo test sequence, then
 *              continuously reads the potentiometer and updates servo position.
 *
 * @return      int     Application exit code (never returns in normal operation)
 */
int main(void) {
	Lpi2c_Ip_StatusType i2cStatus;
	uint8 modeReg;
	uint16 adcValue;

	/*---------------------------------------------------------------------------
	 * Clock Initialization
	 *--------------------------------------------------------------------------*/
	Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

	/*---------------------------------------------------------------------------
	 * OS Interface Initialization
	 *--------------------------------------------------------------------------*/
	OsIf_Init(NULL_PTR);

	/*---------------------------------------------------------------------------
	 * Port Initialization
	 *--------------------------------------------------------------------------*/
	Siul2_Port_Ip_Init(
			NUM_OF_CONFIGURED_PINS_PortContainer_0_BOARD_InitPeripherals,
			g_pin_mux_InitConfigArr_PortContainer_0_BOARD_InitPeripherals);

	/*---------------------------------------------------------------------------
	 * I2C Initialization
	 *--------------------------------------------------------------------------*/
	Lpi2c_Ip_MasterInit(PCA9685_I2C_INSTANCE,
			&I2c_Lpi2cMaster_HwChannel1_Channel0);

	/*---------------------------------------------------------------------------
	 * ADC Initialization
	 *--------------------------------------------------------------------------*/
	Adc_Sar_Ip_Init(ADC_INSTANCE, &AdcHwUnit_0);
	IntCtrl_Ip_InstallHandler(ADC0_IRQn, Adc_Sar_0_Isr, NULL_PTR);
	IntCtrl_Ip_EnableIrq(ADC0_IRQn);

	/* ADC Calibration - run multiple times for accuracy */
	for (uint8 i = 0U; i < 6U; i++) {
		Adc_Sar_Ip_DoCalibration(ADC_INSTANCE);
	}

	/* Enable ADC end-of-chain notification */
	Adc_Sar_Ip_EnableNotifications(ADC_INSTANCE,
			ADC_SAR_IP_NOTIF_FLAG_NORMAL_ENDCHAIN);

	/*---------------------------------------------------------------------------
	 * PCA9685 PWM Controller Initialization
	 *--------------------------------------------------------------------------*/
	i2cStatus = PCA9685_Init();
	if (i2cStatus != LPI2C_IP_SUCCESS_STATUS) {
		/* Handle initialization error - halt execution */
		while (1) {
			Delay_ms(100U);
		}
	}

	/* Verify PCA9685 is responding correctly */
	i2cStatus = PCA9685_ReadReg(PCA9685_REG_MODE1, &modeReg);
	if (i2cStatus != LPI2C_IP_SUCCESS_STATUS) {
		/* Handle communication error - halt execution */
		while (1) {
			Delay_ms(100U);
		}
	}

	/*---------------------------------------------------------------------------
	 * Servo Test Sequence
	 *--------------------------------------------------------------------------*/
	PCA9685_ServoTest(SERVO_CHANNEL);

	/*---------------------------------------------------------------------------
	 * Main Application Loop
	 *--------------------------------------------------------------------------*/
	for (;;) {
		/* Read potentiometer value from ADC */
		adcValue = ReadAdc();
		if (adcValue > adcMaxValue) {
			adcMaxValue = adcValue;
		}
		/* Update servo position based on potentiometer value */
		PCA9685_SetServoPosition(SERVO_CHANNEL, adcValue, adcMaxValue);

		/* Delay for servo stability and loop timing */
		Delay_ms(MAIN_LOOP_DELAY_MS);
	}
}

#ifdef __cplusplus
}
#endif

/** @} */
