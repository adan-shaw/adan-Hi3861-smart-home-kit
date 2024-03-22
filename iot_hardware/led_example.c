#include <stdio.h>

#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"

#define LED_INTERVAL_TIME_US 300000
#define LED_TASK_STACK_SIZE 512
#define LED_TASK_PRIO 25
#define LED_TEST_GPIO 9 // for hispark_pegasus
enum LedState
{
	LED_ON = 0,
	LED_OFF,
	LED_SPARK,
};

enum LedState g_ledState = LED_SPARK;

static void *LedTask (const char *arg)
{
	(void) arg;
	while (1)
	{
		switch (g_ledState)
		{
		case LED_ON:
			IoTGpioSetOutputVal (LED_TEST_GPIO, 1);
			usleep (LED_INTERVAL_TIME_US);
			break;
		case LED_OFF:
			IoTGpioSetOutputVal (LED_TEST_GPIO, 0);
			usleep (LED_INTERVAL_TIME_US);
			break;
		case LED_SPARK:
			IoTGpioSetOutputVal (LED_TEST_GPIO, 0);
			usleep (LED_INTERVAL_TIME_US);
			IoTGpioSetOutputVal (LED_TEST_GPIO, 1);
			usleep (LED_INTERVAL_TIME_US);
			break;
		default:
			usleep (LED_INTERVAL_TIME_US);
			break;
		}
	}

	return NULL;
}

static void LedExampleEntry (void)
{
	osThreadAttr_t attr;

	IoTGpioInit (LED_TEST_GPIO);
	IoTGpioSetDir (LED_TEST_GPIO, IOT_GPIO_DIR_OUT);

	attr.name = "LedTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = LED_TASK_STACK_SIZE;
	attr.priority = LED_TASK_PRIO;

	if (osThreadNew ((osThreadFunc_t) LedTask, NULL, &attr) == NULL)
	{
		printf ("[LedExample] Falied to create LedTask!\n");
	}
}

SYS_RUN (LedExampleEntry);
