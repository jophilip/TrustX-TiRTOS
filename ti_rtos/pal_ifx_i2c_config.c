/**
* MIT License
*
* Copyright (c) 2020 
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements platform abstraction layer configurations for ifx i2c protocol.
*
* \ingroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_i2c.h"
#include "ti_drivers_config.h"

/*********************************************************************************************************************
 * pal ifx i2c instance
 *********************************************************************************************************************/
/*!< gpio number for I2C master clock */
#define PAL_I2C_MASTER_SCL_IO           CONFIG_PIN_2

/*!< gpio number for I2C master data  */
#define PAL_I2C_MASTER_SDA_IO           CONFIG_PIN_1

/*!< gpio number for software reset line  */
#define PAL_I2C_MASTER_RESET            CONFIG_PIN_3

/*!< I2C port number for master dev */
#define PAL_I2C_MASTER_NUM              CONFIG_I2C_0

/*!< I2C master clock frequency */
#define PAL_I2C_MASTER_FREQ_HZ          400000


typedef struct ti_i2c_ctx {
	uint8_t  port;
	uint8_t	 scl_io;
	uint8_t	 sda_io;
	uint32_t bitrate;
}ti_i2c_ctx_t;

ti_i2c_ctx_t	ti_i2c_ctx_0 = {PAL_I2C_MASTER_NUM,
                                PAL_I2C_MASTER_SCL_IO,
								PAL_I2C_MASTER_SDA_IO,
								PAL_I2C_MASTER_FREQ_HZ};

ti_i2c_ctx_t ti_i2c_ctx_0;

/**
 * \brief PAL I2C configuration for OPTIGA. 
 */
pal_i2c_t optiga_pal_i2c_context_0 =
{
    /// Pointer to I2C master platform specific context
    (void*)&ti_i2c_ctx_0,
    /// Slave address
    0x30,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL
};

/*********************************************************************************************************************
 * PAL GPIO configurations defined for Ti RTOS
 *********************************************************************************************************************/
/**
* \brief PAL vdd pin configuration for OPTIGA. 
 */
pal_gpio_t optiga_vdd_0 =
{
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void*)NULL  
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 =
{
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void*)PAL_I2C_MASTER_RESET
};


/**
* @}
*/

