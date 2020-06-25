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
* \brief This file implements the platform abstraction layer APIs for os event/scheduler.
*
* \ingroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#include "optiga/pal/pal_os_event.h"

#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden 

typedef struct callbacks {
    /// Callback function when timer elapses
        volatile register_callback func;
        /// Pointer to store upper layer callback context (For example: Ifx i2c context)
        void * args;
    }pal_os_event_clbs_t;

static pal_os_event_clbs_t clb_ctx_0;
static Timer_Handle timer0;

#define TASK_STACK_SIZE 512

/* Task's stack */
uint8_t appTaskStack[TASK_STACK_SIZE];

/* Task object (to be constructed) */
Task_Struct task1;


Semaphore_Struct semStruct;
Semaphore_Handle semHandle;

bool doTask = false;
/**
 * This callback is called every time_us microseconds. 
 *
 */
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
/*    pal_os_event_clbs_t clb_params;
    clb_params.clb = clbs.clb;
    clb_params.clb_ctx = clbs.clb_ctx;
    Queue_enqueue(qCallback, &clb_params._elem);*/
    if (Semaphore_getCount(semHandle) == 0) {
        System_printf("Sem blocked in task1\n");
    }
    doTask = true;
    Semaphore_post(semHandle);
    if (Semaphore_getCount(semHandle) == 0) {
        System_printf("Sem blocked in task1\n");
    }
}

void vTaskCallbackHandler(UArg arg0, UArg arg1)
{
    register_callback func = NULL;
    void * func_args = NULL;

    if (Semaphore_getCount(semHandle) == 0) {
        System_printf("Sem blocked in task1\n");
    }

    do{
        if(Semaphore_pend(semHandle, BIOS_WAIT_FOREVER))
        {
            if (clb_ctx_0.func){
                func = clb_ctx_0.func;
                func_args = clb_ctx_0.args;
                func((void*)func_args);
            }
        }
    }while(1);
}

/**
* Platform specific event init function.
* <br>
*
* <b>API Details:</b>
*         This function initialise all required event related variables.<br>
*
*
*/
pal_status_t pal_os_event_init(void)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    do{
        Task_Params taskParams;
        Semaphore_Params semParams;
        /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
        Semaphore_Params_init(&semParams);
        semParams.mode  = ti_sysbios_knl_Semaphore_Mode_BINARY;
        Semaphore_construct(&semStruct, 1, &semParams);

        /* Obtain instance handle */
        semHandle = Semaphore_handle(&semStruct);
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
        // Configure task
        Task_Params_init(&taskParams);
        taskParams.stack = appTaskStack;
        taskParams.stackSize = TASK_STACK_SIZE;
        taskParams.priority = 5;
        Task_construct(&task1, vTaskCallbackHandler, &taskParams, NULL);

        Timer_Params params;

        Timer_Params_init(&params);
        params.period = 10;
        params.periodUnits = Timer_PERIOD_COUNTS;
        params.timerMode = Timer_ONESHOT_CALLBACK;
        params.timerCallback = timerCallback;

        timer0 = Timer_open(CONFIG_TIMER_0, &params);

        if (timer0 == NULL) {
            break;
        }

        if (Timer_start(timer0) == Timer_STATUS_ERROR) {
            break;
        }
        else{
            status = PAL_STATUS_SUCCESS;
        }
    }while(0);
    return status;
}

/**
* Platform specific event call back registration function to trigger once when timer expires.
* <br>
*
* <b>API Details:</b>
*         This function registers the callback function supplied by the caller.<br>
*         It triggers a timer with the supplied time interval in microseconds.<br>
*         Once the timer expires, the registered callback function gets called.<br>
* 
* \param[in] callback              Callback function pointer
* \param[in] callback_args         Callback arguments
* \param[in] time_us               time in micro seconds to trigger the call back
*
*/
void pal_os_event_register_callback_oneshot(register_callback callback, 
                                            void* callback_args, 
                                            uint32_t time_us)
{
    if (time_us < 1000) {
        time_us = 1000;
    }
    int32_t timerStatus;
    clb_ctx_0.func = callback;
    clb_ctx_0.args = callback_args;
    //lint --e{534} suppress "Return value is not required to be checked"

    /*Timer_Params params;

    Timer_Params_init(&params);
    params.period = time_us;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_ONESHOT_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL) {
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
    }*/

    Timer_stop(timer0);
    timerStatus = Timer_setPeriod(timer0, Timer_PERIOD_COUNTS, time_us);
    if(timerStatus == Timer_STATUS_SUCCESS){
        if (Timer_start(timer0) == Timer_STATUS_ERROR) {
            // Error starting the timer
        }
    }
    else{
        // unable to set Timer Period
    }

}

/**
* @}
*/

