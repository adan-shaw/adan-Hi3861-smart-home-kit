#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include <unistd.h>
#include <hi_types_base.h>

#include <hi_adc.h>
#include <hi_stdlib.h>
#include <hi_early_debug.h>
#include "iot_gpio.h"

//LDE灯
#define LED_TEST_GPIO 9 // for hispark_pegasus

//光感传感器IO
#define LIGHT_SENSOR_CHAN_NAME HI_ADC_CHANNEL_4

static void ADCLightTask (void *arg)
{
	(void) arg;

	//初始化GPIO
	IoTGpioInit (LED_TEST_GPIO);

	//设置为输出
	IoTGpioSetDir (LED_TEST_GPIO, IOT_GPIO_DIR_OUT);

	while (1)
	{
		unsigned short data = 0;

		//读取光敏传感器得数值
		if (hi_adc_read (LIGHT_SENSOR_CHAN_NAME, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0) == HI_ERR_SUCCESS)
		{
			printf ("ADC_VALUE = %d\n", (unsigned int) data);

			if (data > 125)
			{
				//输出高电平
				IoTGpioSetDir (LED_TEST_GPIO, 1);
			}
			else
			{
				//输出低电平
				IoTGpioSetDir (LED_TEST_GPIO, 0);
			}
			osDelay (10);
		}

	}
}

static void ADCLightDemo (void)
{
	osThreadAttr_t attr;

	attr.name = "ADCLightTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 4096;
	attr.priority = osPriorityNormal;

	if (osThreadNew (ADCLightTask, NULL, &attr) == NULL)
	{
		printf ("[ADCLightDemo] Falied to create ADCLightTask!\n");
	}
}

APP_FEATURE_INIT (ADCLightDemo);
