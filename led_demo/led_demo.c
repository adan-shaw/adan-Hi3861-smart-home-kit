#include <unistd.h>
#include "stdio.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"

#define LED_TEST_GPIO 9					// for hispark_pegasus

void *LedTask (const char *arg)
{
	//初始化GPIO
	IoTGpioInit (LED_TEST_GPIO);

	//设置为输出
	IoTGpioSetDir (LED_TEST_GPIO, IOT_GPIO_DIR_OUT);

	(void) arg;
	while (1)
	{
		//输出低电平
		IoTGpioSetDir (LED_TEST_GPIO, 0);
		usleep (300000);
		//输出高电平
		IoTGpioSetDir (LED_TEST_GPIO, 1);
		usleep (300000);
	}

	return NULL;
}

void led_demo (void)
{
	osThreadAttr_t attr;

	attr.name = "LedTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 512;
	attr.priority = 26;

	if (osThreadNew ((osThreadFunc_t) LedTask, NULL, &attr) == NULL)
	{
		printf ("[LedExample] Falied to create LedTask!\n");
	}

}

SYS_RUN (led_demo);
