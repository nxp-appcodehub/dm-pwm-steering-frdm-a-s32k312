/*==================================================================================================
 * Project : RTD AUTOSAR 4.9
 * Platform : CORTEXM
 * Peripheral : S32K3XX
 * Dependencies : none
 *
 * Autosar Version : 4.9.0
 * Autosar Revision : ASR_REL_4_9_REV_0000
 * Autosar Conf.Variant :
 * SW Version : 7.0.0
 * Build Version : S32K3_RTD_7_0_0_QLP03_D2512_ASR_REL_4_9_REV_0000_20251210
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
*   @file PCA9685.h
*
*   @brief   PCA9685 PWM Controller Driver Header
*   @details This file contains the definitions, macros and function prototypes
*            for the PCA9685 16-channel PWM controller driver.
*
*   @addtogroup PCA9685_MODULE PCA9685 Driver Module
*   @{
*/

#ifndef PCA9685_H
#define PCA9685_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
 *                                        INCLUDE FILES
 * 1) system and project includes
 * 2) needed interfaces from external units
 * 3) internal and external interfaces from this unit
 ==================================================================================================*/
#include "Lpi2c_Ip.h"
#include "Platform_Types.h"

/*==================================================================================================
 *                                      DEFINES AND MACROS
 ==================================================================================================*/

/*-------------------------------------------------------------------------------------------------
 * I2C Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief I2C instance used for PCA9685 communication */
#define PCA9685_I2C_INSTANCE            1U
/** @brief I2C timeout in microseconds */
#define PCA9685_I2C_TIMEOUT_US          100000U

/*-------------------------------------------------------------------------------------------------
 * PCA9685 I2C Address
 *------------------------------------------------------------------------------------------------*/
/** @brief Default I2C address of PCA9685 (A0-A5 all LOW) */
#define PCA9685_ADDRESS                 0x40U

/*-------------------------------------------------------------------------------------------------
 * PCA9685 Register Addresses
 *------------------------------------------------------------------------------------------------*/
/** @brief Mode register 1 */
#define PCA9685_REG_MODE1               0x00U
/** @brief Mode register 2 */
#define PCA9685_REG_MODE2               0x01U
/** @brief I2C-bus subaddress 1 */
#define PCA9685_REG_SUBADR1             0x02U
/** @brief I2C-bus subaddress 2 */
#define PCA9685_REG_SUBADR2             0x03U
/** @brief I2C-bus subaddress 3 */
#define PCA9685_REG_SUBADR3             0x04U
/** @brief LED All Call I2C-bus address */
#define PCA9685_REG_ALLCALLADR          0x05U
/** @brief LED0 output ON low byte */
#define PCA9685_REG_LED0_ON_L           0x06U
/** @brief LED0 output ON high byte */
#define PCA9685_REG_LED0_ON_H           0x07U
/** @brief LED0 output OFF low byte */
#define PCA9685_REG_LED0_OFF_L          0x08U
/** @brief LED0 output OFF high byte */
#define PCA9685_REG_LED0_OFF_H          0x09U
/** @brief All LED ON low byte */
#define PCA9685_REG_ALL_LED_ON_L        0xFAU
/** @brief All LED ON high byte */
#define PCA9685_REG_ALL_LED_ON_H        0xFBU
/** @brief All LED OFF low byte */
#define PCA9685_REG_ALL_LED_OFF_L       0xFCU
/** @brief All LED OFF high byte */
#define PCA9685_REG_ALL_LED_OFF_H       0xFDU
/** @brief Prescaler for PWM output frequency */
#define PCA9685_REG_PRESCALE            0xFEU

/*-------------------------------------------------------------------------------------------------
 * PCA9685 MODE1 Register Bits
 *------------------------------------------------------------------------------------------------*/
/** @brief Restart enabled */
#define PCA9685_MODE1_RESTART           0x80U
/** @brief Use external clock */
#define PCA9685_MODE1_EXTCLK            0x40U
/** @brief Register auto-increment enabled */
#define PCA9685_MODE1_AI                0x20U
/** @brief Low power mode (oscillator off) */
#define PCA9685_MODE1_SLEEP             0x10U
/** @brief Respond to I2C subaddress 1 */
#define PCA9685_MODE1_SUB1              0x08U
/** @brief Respond to I2C subaddress 2 */
#define PCA9685_MODE1_SUB2              0x04U
/** @brief Respond to I2C subaddress 3 */
#define PCA9685_MODE1_SUB3              0x02U
/** @brief Respond to LED All Call address */
#define PCA9685_MODE1_ALLCALL           0x01U

/*-------------------------------------------------------------------------------------------------
 * PCA9685 MODE2 Register Bits
 *------------------------------------------------------------------------------------------------*/
/** @brief Output logic state inverted */
#define PCA9685_MODE2_INVRT             0x10U
/** @brief Outputs change on ACK (vs STOP) */
#define PCA9685_MODE2_OCH               0x08U
/** @brief Totem pole structure output */
#define PCA9685_MODE2_OUTDRV            0x04U
/** @brief Output enable bit 1 */
#define PCA9685_MODE2_OUTNE1            0x02U
/** @brief Output enable bit 0 */
#define PCA9685_MODE2_OUTNE0            0x01U

/*-------------------------------------------------------------------------------------------------
 * PCA9685 PWM Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief Internal oscillator frequency (25MHz) */
#define PCA9685_OSC_FREQ                25000000UL
/** @brief PWM resolution (12-bit = 4096 steps) */
#define PCA9685_PWM_RESOLUTION          4096U
/** @brief Maximum PWM channel number */
#define PCA9685_MAX_CHANNEL             15U

/*-------------------------------------------------------------------------------------------------
 * Servo Configuration
 *------------------------------------------------------------------------------------------------*/
/** @brief PWM frequency for servo control (50Hz) */
#define PCA9685_SERVO_PWM_FREQ          50U
/** @brief Servo minimum pulse width (~0.5ms = full CCW) */
#define PCA9685_SERVO_MIN_PULSE         102U
/** @brief Servo center pulse width (~1.5ms = center) */
#define PCA9685_SERVO_CENTER_PULSE      307U
/** @brief Servo maximum pulse width (~2.5ms = full CW) */
#define PCA9685_SERVO_MAX_PULSE         512U

/*==================================================================================================
 *                                             ENUMS
 ==================================================================================================*/

/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 ==================================================================================================*/

/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 ==================================================================================================*/

/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 ==================================================================================================*/

/**
 * @brief       Initialize PCA9685 PWM controller
 * @details     Configures the PCA9685 with 50Hz PWM frequency for servo control.
 *              Sets up auto-increment mode and totem pole outputs.
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_Init(void);

/**
 * @brief       Write a value to a PCA9685 register
 * @details     Sends a single byte to the specified register address.
 *
 * @param[in]   reg     Register address to write to
 * @param[in]   value   Value to write
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_WriteReg(uint8 reg, uint8 value);

/**
 * @brief       Read a value from a PCA9685 register
 * @details     Reads a single byte from the specified register address.
 *
 * @param[in]   reg     Register address to read from
 * @param[out]  value   Pointer to store the read value
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_ReadReg(uint8 reg, uint8 *value);

/**
 * @brief       Set PWM output on a specific channel
 * @details     Sets the ON and OFF times for a PWM channel (0-15).
 *              The PWM cycle is 4096 counts (12-bit resolution).
 *
 * @param[in]   channel PWM channel number (0-15)
 * @param[in]   on      12-bit value for ON time (0-4095)
 * @param[in]   off     12-bit value for OFF time (0-4095)
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_SetPWM(uint8 channel, uint16 on, uint16 off);

/**
 * @brief       Set PWM output on all channels
 * @details     Sets the same ON and OFF times for all 16 PWM channels.
 *
 * @param[in]   on      12-bit value for ON time (0-4095)
 * @param[in]   off     12-bit value for OFF time (0-4095)
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_SetAllPWM(uint16 on, uint16 off);

/**
 * @brief       Set servo position based on input value
 * @details     Maps the input value to servo pulse width and sets the PWM output.
 *
 * @param[in]   channel     Servo channel (0-15)
 * @param[in]   inputValue  Input value (0 to inputMax)
 * @param[in]   inputMax    Maximum input value (e.g., 16383 for 14-bit ADC)
 *
 * @return      Lpi2c_Ip_StatusType - LPI2C_IP_SUCCESS_STATUS if successful
 *
 * @api
 */
Lpi2c_Ip_StatusType PCA9685_SetServoPosition(uint8 channel, uint16 inputValue, uint16 inputMax);

/**
 * @brief       Run servo test sequence
 * @details     Moves the servo through min, center, and max positions.
 *
 * @param[in]   channel     Servo channel to test (0-15)
 *
 * @return      void
 *
 * @api
 */
void PCA9685_ServoTest(uint8 channel);

#ifdef __cplusplus
}
#endif

#endif /* PCA9685_H */

/** @} */
