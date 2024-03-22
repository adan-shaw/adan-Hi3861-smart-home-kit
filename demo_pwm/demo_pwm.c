#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include <hi_types_base.h>
#include <hi_early_debug.h>

#include <hi_pwm.h>

hi_void pwm_0_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM0);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM0, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void pwm_1_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM1);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM1, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void pwm_2_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM2);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM2, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void pwm_3_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM3);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM3, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void pwm_4_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM4);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM4, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void pwm_5_demo (hi_void)
{
	hi_pwm_init (HI_PWM_PORT_PWM5);
	hi_pwm_set_clock (PWM_CLK_160M);
	hi_pwm_start (HI_PWM_PORT_PWM5, 750, 1500);/* duty: 750 freq:1500 */
}

hi_void app_demo_pwm (hi_void)
{
	printf ("start test pwm");

	pwm_0_demo ();
	pwm_1_demo ();
	pwm_2_demo ();
	pwm_3_demo ();
	pwm_4_demo ();
	pwm_5_demo ();

	printf ("please use an oscilloscope to check the output waveform!");
}

void *PWM_Task (const char *arg)
{
	arg = arg;

	while (1)
	{
//调用app_demo_pwm
		app_demo_pwm ();
		usleep (10000);
	}
}

void pwm_demo (void)
{
	osThreadAttr_t attr;

	attr.name = "PWM_Task";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 2048;
	attr.priority = 26;

	if (osThreadNew ((osThreadFunc_t) PWM_Task, NULL, &attr) == NULL)
	{
		printf ("[PWM_Task] Falied to create PWM_Task!\n");
	}

}

SYS_RUN (pwm_demo);
