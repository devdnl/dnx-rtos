/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "portmacro.h"

#undef pvPortMalloc
#undef vPortFree
#define pvPortMalloc(size) malloc(size)
#define vPortFree(mem) free(mem)

/*-----------------------------------------------------------*/

#define MAX_NUMBER_OF_TASKS 		( _POSIX_THREAD_THREADS_MAX )
/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting variable. */
typedef struct THREAD_SUSPENSIONS
{
	jmp_buf *ctx;
	xTaskHandle hTask;
	pdTASK_CODE pxCode;
	void *pvParams;
	jmp_buf *pxCaller;
} xThreadState;
/*-----------------------------------------------------------*/

static xThreadState *pxThreads = NULL;
/*-----------------------------------------------------------*/
static sigset_t *pxInterruptsBeforeDisabled = NULL;
static portBASE_TYPE xPendYield = pdFALSE;
static portLONG lIndexOfLastAddedTask = 0;
static unsigned portBASE_TYPE uxCriticalNesting = 0;
static jmp_buf *xMainCtx = NULL;
/*-----------------------------------------------------------*/

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupPxThreads( void );
static void prvSetupTimerInterrupt( void );
static void prvSetupSignalsAndSchedulerPolicy( void );
static void prvResumeTask( xTaskHandle hTaskToResume, xTaskHandle hTaskToSuspend );
static xThreadState *prvGetThreadState( xTaskHandle xTaskHandle );
static portLONG prvGetFreeThreadState( void );
static xThreadState *prvGetThreadStateToStart( void );
/*-----------------------------------------------------------*/

/*
 * Exception handlers.
 */
void vPortYield( void );
void vPortSystemTickHandler( int sig );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
void vPortStartFirstTask( void );
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	vPortEnterCritical();
	prvSetupPxThreads();

	lIndexOfLastAddedTask = prvGetFreeThreadState();
	xThreadState *pxThread = &pxThreads[ lIndexOfLastAddedTask ];
	pxThread->pxCode = pxCode;
	pxThread->pvParams = pvParameters;

	if ( xMainCtx != NULL )
	{
		pxThread->pxCaller = pvPortMalloc( sizeof( jmp_buf ) );

		if ( setjmp( *pxThread->pxCaller ) == 0 )
		{
			longjmp( *xMainCtx, 1 );
		}
		else
		{
			vPortFree( pxThread->pxCaller );
			pxThread->pxCaller = NULL;
		}
	}

	vPortExitCritical();

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortStartFirstTask( void )
{
	/* Start the first task. */
	vPortEnableInterrupts();

	jmp_buf env;
	xMainCtx = &env;
	int result = setjmp(env);
	xThreadState *xThreadToStart;

	while ( ( xThreadToStart = prvGetThreadStateToStart() ) != NULL )
	{
		xThreadToStart->ctx = pvPortMalloc( sizeof( jmp_buf ) );

		if ( setjmp( *xThreadToStart->ctx ) != 0 )
		{
			xThreadToStart = prvGetThreadState( xTaskGetCurrentTaskHandle() );
			xThreadToStart->pxCode( xThreadToStart->pvParams );
			vPortForciblyEndThread( xTaskGetCurrentTaskHandle() );
			printf("vPortForciblyEndThread shouldn't return given that a task is commiting suicide.\n");
			assert( false );
			exit( 1 );
		}
		else
		{
			if ( xThreadToStart->pxCaller != NULL )
			{
				longjmp( *xThreadToStart->pxCaller, 1 );
			}
		}
	}

	if ( result == 0 )
	{
		/* Start the first task. */
		prvResumeTask( xTaskGetCurrentTaskHandle(), NULL );
	}

	xMainCtx = NULL;
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupPxThreads();
	prvSetupTimerInterrupt();
	prvSetupSignalsAndSchedulerPolicy();

	/* Start the first task. Will not return unless all threads are killed. */
	vPortStartFirstTask();

	printf( "Cleaning Up, Exiting.\n" );
	vPortFree( (void *)pxThreads );

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	if (xMainCtx != NULL)
	{
		longjmp(*xMainCtx, 1);
	}
	else
	{
		printf("Cannot end scheduler as it was never started");
		assert(false);
		exit(1);
	}
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
	/* Calling Yield from a Interrupt/Signal handler often doesn't work because the
	 * xSingleThreadMutex is already owned by an original call to Yield. Therefore,
	 * simply indicate that a yield is required soon.
	 */
	xPendYield = pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	vPortDisableInterrupts();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	/* Check for unmatched exits. */
	if ( uxCriticalNesting > 0 )
	{
		uxCriticalNesting--;
	}

	/* If we have reached 0 then re-enable the interrupts. */
	if( uxCriticalNesting == 0 )
	{
		vPortEnableInterrupts();
		vPortYield();
	}
}

static bool prvInteruptsAreEnabled( void )
{
	return pxInterruptsBeforeDisabled == NULL;
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
xTaskHandle xTaskToSuspend, xTaskToResume;
	assert( uxCriticalNesting == 0 );
	assert( prvInteruptsAreEnabled() );

	if ( xMainCtx != NULL )
	{
		xTaskToSuspend = xTaskGetCurrentTaskHandle();
		vTaskSwitchContext();
		xTaskToResume = xTaskGetCurrentTaskHandle();

		if ( xTaskToSuspend != xTaskToResume)
		{
			prvResumeTask( xTaskToResume, xTaskToSuspend );
		}
	}
}
/*-----------------------------------------------------------*/

void vPortDisableInterrupts( void )
{
	assert( prvInteruptsAreEnabled() );
	sigset_t *pxOldInterrupts = pvPortMalloc( sizeof( sigset_t ) );
	sigset_t pxInterruptsDisabled;
	(void) sigfillset( &pxInterruptsDisabled );
	sigprocmask( SIG_BLOCK, &pxInterruptsDisabled, pxOldInterrupts );
	pxInterruptsBeforeDisabled = pxOldInterrupts;
}
/*-----------------------------------------------------------*/

void vPortEnableInterrupts( void )
{
	assert( !prvInteruptsAreEnabled() );
	sigset_t *pxOldInterrupts = pxInterruptsBeforeDisabled;
	pxInterruptsBeforeDisabled = NULL;
	sigprocmask( SIG_SETMASK, pxOldInterrupts, NULL );
	vPortFree( pxOldInterrupts );
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortSetInterruptMask( void )
{
portBASE_TYPE xReturn = prvInteruptsAreEnabled();
	if ( xReturn )
	{
		vPortDisableInterrupts();
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

void vPortClearInterruptMask( portBASE_TYPE xMask )
{
	if ( xMask )
	{
		if( !prvInteruptsAreEnabled() )
		{
			vPortEnableInterrupts();
		}
	}
	else
	{
		if( prvInteruptsAreEnabled() )
		{
			vPortDisableInterrupts();
		}
	}
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
struct itimerval itimer, oitimer;
portTickType xMicroSeconds = portTICK_RATE_MICROSECONDS;

	/* Initialise the structure with the current timer information. */
	if ( 0 == getitimer( TIMER_TYPE, &itimer ) )
	{
		/* Set the interval between timer events. */
		itimer.it_interval.tv_sec = 0;
		itimer.it_interval.tv_usec = xMicroSeconds;

		/* Set the current count-down. */
		itimer.it_value.tv_sec = 0;
		itimer.it_value.tv_usec = xMicroSeconds;

		/* Set-up the timer interrupt. */
		if ( 0 != setitimer( TIMER_TYPE, &itimer, &oitimer ) )
		{
			assert( false );
			printf( "Set Timer problem.\n" );
			exit( 1 );
		}
	}
	else
	{
		assert( false );
		printf( "Get Timer problem.\n" );
		exit( 1 );
	}
}
/*-----------------------------------------------------------*/

void vPortSystemTickHandler( int sig )
{
	(void) sig;

#if ( configUSE_PREEMPTION == 1 )
	vPortEnterCritical();
#else
	vPortDisableInterupts();
#endif
	/* Tick Increment. */
	vTaskIncrementTick();
#if ( configUSE_PREEMPTION == 1 )
	vPortExitCritical();
#else
	vPortEnableInterupts();
#endif
}
/*-----------------------------------------------------------*/

void vPortForciblyEndThread( void *pxTaskToDelete )
{
xTaskHandle hTaskToDelete = ( xTaskHandle )pxTaskToDelete;
xTaskHandle hTaskToResume = xTaskGetCurrentTaskHandle();
xThreadState *xThreadStateToDelete = prvGetThreadState( hTaskToDelete );

	xThreadStateToDelete->hTask = NULL;
	xThreadStateToDelete->pxCode = NULL;
	xThreadStateToDelete->pvParams = NULL;

	if ( xThreadStateToDelete->ctx != NULL )
	{
		vPortFree( xThreadStateToDelete->ctx );
		xThreadStateToDelete->ctx = NULL;
	}

	if ( xThreadStateToDelete->pxCaller != NULL )
	{
		vPortFree( xThreadStateToDelete->pxCaller );
		xThreadStateToDelete->pxCaller = NULL;
	}

	if ( hTaskToResume == hTaskToDelete )
	{
		/* This is a suicidal thread, need to select a different task to run. */
		vTaskSwitchContext();
		hTaskToResume = xTaskGetCurrentTaskHandle();
		assert( hTaskToResume != hTaskToDelete );
		hTaskToResume = NULL;

		if (uxCriticalNesting != 0)
		{
			uxCriticalNesting = 0;
			vPortEnableInterrupts();
		}
	}

	if ( hTaskToResume != NULL )
	{
		prvResumeTask( hTaskToResume, NULL );
	}
	else
	{
		vPortEndScheduler();
	}
}
/*-----------------------------------------------------------*/

void prvResumeTask( xTaskHandle hTaskToResume, xTaskHandle hTaskToSuspend )
{
xThreadState *xThreadStateToSuspend;
xThreadState *xThreadStateToResume = prvGetThreadState( hTaskToResume );
int result = 0;

	assert( xThreadStateToResume != NULL );

	if ( hTaskToSuspend != NULL )
	{
		xThreadStateToSuspend = prvGetThreadState( hTaskToSuspend );
		assert( xThreadStateToSuspend != NULL );
		result = setjmp( *xThreadStateToSuspend->ctx );
	}

	if ( result == 0 )
	{
		longjmp( *xThreadStateToResume->ctx, 1 );
	}
}
/*-----------------------------------------------------------*/

void prvSetupSignalsAndSchedulerPolicy( void )
{
/* The following code would allow for configuring the scheduling of this task as a Real-time task.
 * The process would then need to be run with higher privileges for it to take affect.
int iPolicy;
int iResult;
int iSchedulerPriority;
	iResult = pthread_getschedparam( pthread_self(), &iPolicy, &iSchedulerPriority );
	iResult = pthread_attr_setschedpolicy( &xThreadAttributes, SCHED_FIFO );
	iPolicy = SCHED_FIFO;
	iResult = pthread_setschedparam( pthread_self(), iPolicy, &iSchedulerPriority );		*/

struct sigaction sigtick;

	sigtick.sa_flags = 0;
	sigtick.sa_handler = vPortSystemTickHandler;
	sigfillset( &sigtick.sa_mask );

	if ( 0 != sigaction( SIG_TICK, &sigtick, NULL ) )
	{
		printf( "Problem installing SIG_TICK\n" );
	}

	printf( "Running as PID: %d\n", getpid() );
}

xThreadState *prvGetThreadState( xTaskHandle hTask )
{
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hTask == hTask )
		{
			return &pxThreads[ lIndex ];
		}
	}
	return NULL;
}

/*-----------------------------------------------------------*/

portLONG prvGetFreeThreadState( void )
{
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hTask == NULL &&
				pxThreads[ lIndex ].ctx == NULL &&
				pxThreads[ lIndex ].pxCode == NULL &&
				pxThreads[ lIndex ].pvParams == NULL &&
				pxThreads[ lIndex ].pxCaller == NULL )
		{
			return lIndex;
		}
	}

	printf( "No more free threads, please increase the maximum.\n" );
	vPortEndScheduler();

	return MAX_NUMBER_OF_TASKS;
}
/*-----------------------------------------------------------*/

void prvSetupPxThreads(void)
{
portLONG lIndex;

	if (pxThreads == NULL)
	{
		pxThreads = ( xThreadState *) pvPortMalloc( sizeof( xThreadState ) * MAX_NUMBER_OF_TASKS );
		for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
		{
			pxThreads[ lIndex ].hTask = NULL;
			pxThreads[ lIndex ].ctx = NULL;
			pxThreads[ lIndex ].pxCode = NULL;
			pxThreads[ lIndex ].pvParams = NULL;
			pxThreads[ lIndex ].pxCaller = NULL;
		}
	}
}

xThreadState *prvGetThreadStateToStart( void )
{
portLONG lIndex;

	if (pxThreads != NULL)
	{
		for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
		{
			if ( pxThreads[ lIndex ].ctx == NULL &&
					pxThreads[ lIndex ].pxCode != NULL )
			{
				return &pxThreads[ lIndex ];
			}
		}
	}

	return NULL;
}

void vPortAddTaskHandle( void *pxTaskHandle )
{
	assert( pxThreads[ lIndexOfLastAddedTask ].hTask == NULL );
	pxThreads[ lIndexOfLastAddedTask ].hTask = ( xTaskHandle )pxTaskHandle;
}
/*-----------------------------------------------------------*/

void vPortFindTicksPerSecond( void )
{
	/* Needs to be reasonably high for accuracy. */
	unsigned long ulTicksPerSecond = sysconf(_SC_CLK_TCK);
	printf( "Timer Resolution for Run TimeStats is %ld ticks per second.\n", ulTicksPerSecond );
}
/*-----------------------------------------------------------*/

unsigned long ulPortGetTimerValue( void )
{
struct tms xTimes;
	unsigned long ulTotalTime = times( &xTimes );
	/* Return the application code times.
	 * The timer only increases when the application code is actually running
	 * which means that the total execution times should add up to 100%.
	 */
	return ( unsigned long ) xTimes.tms_utime;

	/* Should check ulTotalTime for being clock_t max minus 1. */
	(void)ulTotalTime;
}
/*-----------------------------------------------------------*/

//void *pvPortMalloc(size_t size)
//{
//	return malloc(size);
//}
//
//void vPortFree(void *ptr)
//{
//	free(ptr);
//}
