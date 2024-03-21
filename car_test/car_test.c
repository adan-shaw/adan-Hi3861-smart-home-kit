#include <stdio.h>

#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include <iot_pwm.h>
#include <iot_gpio.h>

#include <unistd.h>
#include <hi_types_base.h>
#include <hi_early_debug.h>

#include <hi_pwm.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include "car_test.h"

#include "iot_pwm.h"

#define GPIO0 0
#define GPIO1 1
#define GPIO9 9
#define GPIO10 10
#define GPIOFUNC 0
#define PWM_FREQ_FREQUENCY  (60000)

void gpio_control (unsigned int gpio, IotGpioValue value)
{
	hi_io_set_func (gpio, GPIOFUNC);
	IoTGpioSetDir (gpio, IOT_GPIO_DIR_OUT);
	IoTGpioSetOutputVal (gpio, value);
}

struct car_sys_info car_info;

//CarStatus carstatus = CAR_STATUS_STOP;
//CarMode carmode = CAR_MODE_STEP;

void car_info_init (void)
{
	car_info.go_status = CAR_STATUS_STOP;
	car_info.cur_status = CAR_STATUS_STOP;

	car_info.mode = CAR_MODE_STEP;

	car_info.step_count = CAR_STEP_COUNT;
}

void step_count_update (void)
{
	car_info.step_count = CAR_STEP_COUNT;
}

void set_car_status (CarStatus status)
{
	if (status != car_info.cur_status)
	{
		car_info.status_change = 1;
	}
	car_info.go_status = status;

	step_count_update ();
}

//璁剧疆琛岄┒妯″紡
void set_car_mode (CarMode mode)
{
	car_info.mode = mode;
}

void pwm_init (void)
{
	IoTGpioInit (IO_NAME_GPIO_0);
	IoTGpioInit (IO_NAME_GPIO_1);
	IoTGpioInit (IO_NAME_GPIO_9);
	IoTGpioInit (IO_NAME_GPIO_10);

	//寮曡剼澶嶇敤
	//hi_io_set_func(IO_NAME_GPIO_0, IO_FUNC_GPIO_0_PWM3_OUT); 
	//hi_io_set_func(IO_NAME_GPIO_1, IO_FUNC_GPIO_1_PWM4_OUT);  
	//hi_io_set_func(IO_NAME_GPIO_9, IO_FUNC_GPIO_9_PWM0_OUT); 
	//hi_io_set_func(IO_NAME_GPIO_10, IO_FUNC_GPIO_10_PWM1_OUT); 

	//鍒濆鍖杙wm
	//IoTPwmInit(PWM_PORT_PWM3);
	//IoTPwmInit(PWM_PORT_PWM4);
	//IoTPwmInit(PWM_PORT_PWM0);
	//IoTPwmInit(PWM_PORT_PWM1);

}

//鍋滄
void pwm_stop (void)
{
	//鍏堝仠姝WM
	gpio_control (GPIO0, IOT_GPIO_VALUE1);
	gpio_control (GPIO1, IOT_GPIO_VALUE1);
	gpio_control (GPIO9, IOT_GPIO_VALUE1);
	gpio_control (GPIO10, IOT_GPIO_VALUE1);
}

void car_stop (void)
{
	car_info.cur_status = car_info.go_status;

	printf ("pwm_stop \r\n");

	pwm_stop ();
}

//鍓嶈繘
void pwm_forward (void)
{
	//鍏堝仠姝WM
	gpio_control (GPIO0, IOT_GPIO_VALUE1);
	gpio_control (GPIO1, IOT_GPIO_VALUE0);
	gpio_control (GPIO9, IOT_GPIO_VALUE1);
	gpio_control (GPIO10, IOT_GPIO_VALUE0);

	//鍚姩A璺疨WM
	//左轮
	//IoTPwmStart(PWM_PORT_PWM3, 64000, 64000);
	//右轮
	//IoTPwmStart(PWM_PORT_PWM0, 64000, 64000);
}

void car_forward (void)
{
	if (car_info.go_status != CAR_STATUS_FORWARD)
	{
		//鐩存帴閫€鍑?
		return;
	}
	if (car_info.cur_status == car_info.go_status)
	{
		//鐘舵€佹病鏈夊彉鍖栵紝鐩存帴鎺ㄥ嚭
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf ("pwm_forward \r\n");

	pwm_forward ();

	step_count_update ();
}

//鍚庨€€
void pwm_backward (void)
{
	//鍏堝仠姝WM
	gpio_control (GPIO0, IOT_GPIO_VALUE0);
	gpio_control (GPIO1, IOT_GPIO_VALUE1);
	gpio_control (GPIO9, IOT_GPIO_VALUE0);
	gpio_control (GPIO10, IOT_GPIO_VALUE1);

	//鍚姩A璺疨WM
	//IoTPwmStart(PWM_PORT_PWM4, 64000, 64000);
	//IoTPwmStart(PWM_PORT_PWM1, 64000, 64000);
}

void car_backward (void)
{
	if (car_info.go_status != CAR_STATUS_BACKWARD)
	{
		//鐩存帴閫€鍑?
		return;
	}
	if (car_info.cur_status == car_info.go_status)
	{
		//鐘舵€佹病鏈夊彉鍖栵紝鐩存帴鎺ㄥ嚭
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf ("pwm_backward \r\n");

	pwm_backward ();

	step_count_update ();
}

//宸﹁浆
void pwm_left (void)
{
	//鍏堝仠姝WM
	gpio_control (GPIO0, IOT_GPIO_VALUE0);
	gpio_control (GPIO1, IOT_GPIO_VALUE0);
	gpio_control (GPIO9, IOT_GPIO_VALUE1);
	gpio_control (GPIO10, IOT_GPIO_VALUE0);

	//鍚姩A璺疨WM
	//IoTPwmStart(PWM_PORT_PWM0, 64000, 64000);

}

void car_left (void)
{
	if (car_info.go_status != CAR_STATUS_LEFT)
	{
		//鐩存帴閫€鍑?
		return;
	}
	if (car_info.cur_status == car_info.go_status)
	{
		//鐘舵€佹病鏈夊彉鍖栵紝鐩存帴鎺ㄥ嚭
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf ("pwm_left \r\n");

	pwm_left ();

	step_count_update ();
}

//鍙宠浆
void pwm_right (void)
{
	//鍏堝仠姝WM
	gpio_control (GPIO0, IOT_GPIO_VALUE1);
	gpio_control (GPIO1, IOT_GPIO_VALUE0);
	gpio_control (GPIO9, IOT_GPIO_VALUE0);
	gpio_control (GPIO10, IOT_GPIO_VALUE0);

	//鍚姩A璺疨WM
	//IoTPwmStart(PWM_PORT_PWM3, 64000, 64000);
}

void car_right (void)
{
	if (car_info.go_status != CAR_STATUS_RIGHT)
	{
		//鐩存帴閫€鍑?
		return;
	}
	if (car_info.cur_status == car_info.go_status)
	{
		//鐘舵€佹病鏈夊彉鍖栵紝鐩存帴鎺ㄥ嚭
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf ("pwm_right \r\n");

	pwm_right ();

	step_count_update ();
}

extern void start_udp_thread (void);

void car_test (void)
{
	start_udp_thread ();
	pwm_init ();
	car_info_init ();
	//set_car_status(CAR_STATUS_FORWARD);
	//set_car_mode(CAR_MODE_ALWAY);

	pwm_forward ();
	sleep (1);

	pwm_backward ();
	sleep (1);

	pwm_right ();
	sleep (1);

	pwm_left ();
	sleep (1);

	pwm_stop ();

	while (1)
	{
		if (car_info.status_change)
		{
			car_info.status_change = 0;

			switch (car_info.go_status)
			{
			case CAR_STATUS_STOP:
				car_stop ();
				break;

			case CAR_STATUS_FORWARD:
				car_forward ();
				break;

			case CAR_STATUS_BACKWARD:
				car_backward ();
				break;

			case CAR_STATUS_LEFT:
				car_left ();
				break;

			case CAR_STATUS_RIGHT:
				car_right ();
				break;

			default:

				break;
			}
		}

		if (car_info.mode == CAR_MODE_STEP)
		{
			if (car_info.go_status != CAR_STATUS_STOP)
			{
				if (car_info.step_count > 0)
				{
					car_info.step_count--;
				}
				else
				{
					printf ("stop... \r\n");
					set_car_status (CAR_STATUS_STOP);
				}
			}
		}

		usleep (1000);
	}

}
