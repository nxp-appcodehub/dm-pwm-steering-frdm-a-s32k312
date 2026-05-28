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
*   @file PCA9685.c
*
*   @brief   PCA9685 PWM Controller Driver Implementation
*   @details This file contains the implementation of the PCA9685 16-channel
*            PWM controller driver for servo motor control.
*
*   @addtogroup PCA9685_MODULE PCA9685 Driver Module
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
#include "PCA9685.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 ==================================================================================================*/

/*==================================================================================================
 *                                       LOCAL MACROS
 ==================================================================================================*/

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 ==================================================================================================*/

/*==================================================================================================
 *                                      LOCAL VARIABLES
 ==================================================================================================*/

/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 ==================================================================================================*/

/*==================================================================================================
 *                                      GLOBAL VARIABLES
 ==================================================================================================*/

/*==================================================================================================
 *                                   LOCAL FUNCTION PROTOTYPES
 ==================================================================================================*/
static void PCA9685_DelayMs(uint32 ms);
static uint16 PCA9685_MapValue(uint16 value, uint16 inMin, uint16 inMax, uint16 outMin, uint16 outMax);

/*==================================================================================================
 *                                       LOCAL FUNCTIONS
 ==================================================================================================*/

/**
 * @brief       Software delay in milliseconds
 * @details     Implements a busy-wait delay using NOP instructions.
 *
 * @param[in]   ms  Number of milliseconds to delay
 *
 * @return      void
 */
static void PCA9685_DelayMs(uint32 ms)
{
    volatile uint32 i, j;
    for (i = 0U; i < ms; i++)
    {
        for (j = 0U; j < 16000U; j++)
        {
            __asm volatile ("nop");
        }
    }
}

/**
 * @brief       Map a value from one range to another
 * @details     Performs linear interpolation to map input range to output range.
 *
 * @param[in]   value   Input value to map
 * @param[in]   inMin   Minimum input value
 * @param[in]   inMax   Maximum input value
 * @param[in]   outMin  Minimum output value
 * @param[in]   outMax  Maximum output value
 *
 * @return      uint16  Mapped output value
 */
static uint16 PCA9685_MapValue(uint16 value, uint16 inMin, uint16 inMax, uint16 outMin, uint16 outMax)
{
    if (value <= inMin) return outMin;
    if (value >= inMax) return outMax;
    return (uint16)(((uint32)(value - inMin) * (uint32)(outMax - outMin)) / (uint32)(inMax - inMin) + outMin);
}

/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 ==================================================================================================*/

/**
 * @brief       Write a value to PCA9685 register
 * @implements  PCA9685_WriteReg_Activity
 */
Lpi2c_Ip_StatusType PCA9685_WriteReg(uint8 reg, uint8 value)
{
    uint8 txBuffer[2];
    txBuffer[0] = reg;
    txBuffer[1] = value;

    Lpi2c_Ip_MasterSetSlaveAddr(PCA9685_I2C_INSTANCE, PCA9685_ADDRESS, FALSE);
    return Lpi2c_Ip_MasterSendDataBlocking(PCA9685_I2C_INSTANCE, txBuffer, 2U, TRUE, PCA9685_I2C_TIMEOUT_US);
}

/**
 * @brief       Read a value from PCA9685 register
 * @implements  PCA9685_ReadReg_Activity
 */
Lpi2c_Ip_StatusType PCA9685_ReadReg(uint8 reg, uint8 *value)
{
    Lpi2c_Ip_StatusType status;

    Lpi2c_Ip_MasterSetSlaveAddr(PCA9685_I2C_INSTANCE, PCA9685_ADDRESS, FALSE);

    /* Send register address */
    status = Lpi2c_Ip_MasterSendDataBlocking(PCA9685_I2C_INSTANCE, &reg, 1U, FALSE, PCA9685_I2C_TIMEOUT_US);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    /* Read register value */
    return Lpi2c_Ip_MasterReceiveDataBlocking(PCA9685_I2C_INSTANCE, value, 1U, TRUE, PCA9685_I2C_TIMEOUT_US);
}

/**
 * @brief       Initialize PCA9685 PWM controller with proper sequence
 * @implements  PCA9685_Init_Activity
 */
Lpi2c_Ip_StatusType PCA9685_Init(void)
{
    Lpi2c_Ip_StatusType status;
    uint8 prescale;
    uint8 oldMode;

    /* Set slave address */
    Lpi2c_Ip_MasterSetSlaveAddr(PCA9685_I2C_INSTANCE, PCA9685_ADDRESS, FALSE);

    /* Read current MODE1 */
    status = PCA9685_ReadReg(PCA9685_REG_MODE1, &oldMode);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    /* Put PCA9685 to sleep (required before changing prescaler) */
    status = PCA9685_WriteReg(PCA9685_REG_MODE1, (oldMode & ~PCA9685_MODE1_RESTART) | PCA9685_MODE1_SLEEP);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    PCA9685_DelayMs(5U);

    /* Calculate prescale value for servo PWM frequency (50Hz) */
    /* prescale = round(OSC_FREQ / (4096 * PWM_FREQ)) - 1 */
    /* For 50Hz: 25000000 / (4096 * 50) - 1 = 121 */
    prescale = (uint8)((PCA9685_OSC_FREQ / (PCA9685_PWM_RESOLUTION * PCA9685_SERVO_PWM_FREQ)) - 1U);

    /* Set prescaler (can only be set when SLEEP = 1) */
    status = PCA9685_WriteReg(PCA9685_REG_PRESCALE, prescale);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    PCA9685_DelayMs(5U);

    /* Wake up: Clear SLEEP bit, enable auto-increment */
    status = PCA9685_WriteReg(PCA9685_REG_MODE1, PCA9685_MODE1_AI);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    /* Wait for oscillator to stabilize (500μs minimum) */
    PCA9685_DelayMs(5U);

    /* Restart PWM channels if needed */
    status = PCA9685_WriteReg(PCA9685_REG_MODE1, PCA9685_MODE1_AI | PCA9685_MODE1_RESTART);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    PCA9685_DelayMs(5U);

    /* Configure MODE2: Totem pole outputs (required for driving servos directly) */
    status = PCA9685_WriteReg(PCA9685_REG_MODE2, PCA9685_MODE2_OUTDRV);
    if (status != LPI2C_IP_SUCCESS_STATUS) return status;

    PCA9685_DelayMs(5U);

    /* Turn off all outputs initially */
    status = PCA9685_SetAllPWM(0U, 0U);

    return status;
}

/**
 * @brief       Set PWM output on a specific channel
 * @implements  PCA9685_SetPWM_Activity
 */
Lpi2c_Ip_StatusType PCA9685_SetPWM(uint8 channel, uint16 on, uint16 off)
{
    uint8 txBuffer[5];
    uint8 baseReg;

    if (channel > PCA9685_MAX_CHANNEL) return LPI2C_IP_ERROR_STATUS;

    baseReg = PCA9685_REG_LED0_ON_L + (4U * channel);

    txBuffer[0] = baseReg;
    txBuffer[1] = (uint8)(on & 0xFFU);          /* ON_L */
    txBuffer[2] = (uint8)((on >> 8U) & 0x0FU);  /* ON_H */
    txBuffer[3] = (uint8)(off & 0xFFU);         /* OFF_L */
    txBuffer[4] = (uint8)((off >> 8U) & 0x0FU); /* OFF_H */

    Lpi2c_Ip_MasterSetSlaveAddr(PCA9685_I2C_INSTANCE, PCA9685_ADDRESS, FALSE);
    return Lpi2c_Ip_MasterSendDataBlocking(PCA9685_I2C_INSTANCE, txBuffer, 5U, TRUE, PCA9685_I2C_TIMEOUT_US);
}

/**
 * @brief       Set PWM on all channels
 * @implements  PCA9685_SetAllPWM_Activity
 */
Lpi2c_Ip_StatusType PCA9685_SetAllPWM(uint16 on, uint16 off)
{
    uint8 txBuffer[5];

    txBuffer[0] = PCA9685_REG_ALL_LED_ON_L;
    txBuffer[1] = (uint8)(on & 0xFFU);
    txBuffer[2] = (uint8)((on >> 8U) & 0x0FU);
    txBuffer[3] = (uint8)(off & 0xFFU);
    txBuffer[4] = (uint8)((off >> 8U) & 0x0FU);

    Lpi2c_Ip_MasterSetSlaveAddr(PCA9685_I2C_INSTANCE, PCA9685_ADDRESS, FALSE);
    return Lpi2c_Ip_MasterSendDataBlocking(PCA9685_I2C_INSTANCE, txBuffer, 5U, TRUE, PCA9685_I2C_TIMEOUT_US);
}

/**
 * @brief       Set servo position based on input value
 * @implements  PCA9685_SetServoPosition_Activity
 */
Lpi2c_Ip_StatusType PCA9685_SetServoPosition(uint8 channel, uint16 inputValue, uint16 inputMax)
{
    uint16 pulseWidth;

    /* Map input value to servo pulse width */
    pulseWidth = PCA9685_MapValue(inputValue, 0U, inputMax, PCA9685_SERVO_MIN_PULSE, PCA9685_SERVO_MAX_PULSE);

    /* Set PWM: ON at 0, OFF at pulse width */
    return PCA9685_SetPWM(channel, 0U, pulseWidth);
}

/**
 * @brief       Test servo by sweeping through positions
 * @implements  PCA9685_ServoTest_Activity
 */
void PCA9685_ServoTest(uint8 channel)
{
    /* Move to minimum position */
    PCA9685_SetPWM(channel, 0U, PCA9685_SERVO_MIN_PULSE);
    PCA9685_DelayMs(1000U);

    /* Move to center position */
    PCA9685_SetPWM(channel, 0U, PCA9685_SERVO_CENTER_PULSE);
    PCA9685_DelayMs(1000U);

    /* Move to maximum position */
    PCA9685_SetPWM(channel, 0U, PCA9685_SERVO_MAX_PULSE);
    PCA9685_DelayMs(1000U);

    /* Return to center */
    PCA9685_SetPWM(channel, 0U, PCA9685_SERVO_CENTER_PULSE);
    PCA9685_DelayMs(500U);
}

#ifdef __cplusplus
}
#endif

/** @} */
