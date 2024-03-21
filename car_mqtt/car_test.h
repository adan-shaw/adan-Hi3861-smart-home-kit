#ifndef __CAR_TEST_H__
#define __CAR_TEST_H__

#define CAR_STEP_COUNT 150

typedef enum
{
	/*停止 */
	CAR_STATUS_STOP,

	/*前进 */
	CAR_STATUS_FORWARD,

	/*后退 */
	CAR_STATUS_BACKWARD,

	/*左转 */
	CAR_STATUS_LEFT,

	/*右转 */
	CAR_STATUS_RIGHT,

	/* Maximum value */
	CAR_STATUS_MAX
} CarStatus;

typedef enum
{
	/*收到前进、后退指令后, 只会走一小段距离, 然后停止 */
	CAR_MODE_STEP,

	/*收到前进、后退指令后, 会一直走 */
	CAR_MODE_ALWAY,

	/* Maximum value */
	CAR_MODE_MAX
} CarMode;

struct car_sys_info
{

	CarStatus cur_status;
	CarStatus go_status;
	char status_change;

	CarMode mode;

	int step_count;
};

void car_test (void);

void set_car_status (CarStatus status);

void set_car_mode (CarMode mode);

#define IO_NAME_GPIO_0		0
#define IO_NAME_GPIO_1		1
#define IO_NAME_GPIO_9		9
#define IO_NAME_GPIO_10		10

#define IO_FUNC_GPIO_0_PWM3_OUT	HI_IO_FUNC_GPIO_0_PWM3_OUT
#define IO_FUNC_GPIO_1_PWM4_OUT	HI_IO_FUNC_GPIO_1_PWM4_OUT
#define IO_FUNC_GPIO_9_PWM0_OUT	HI_IO_FUNC_GPIO_9_PWM0_OUT
#define IO_FUNC_GPIO_10_PWM1_OUT	HI_IO_FUNC_GPIO_10_PWM1_OUT

#define PWM_PORT_PWM3	HI_PWM_PORT_PWM3
#define PWM_PORT_PWM4	HI_PWM_PORT_PWM4
#define PWM_PORT_PWM0	HI_PWM_PORT_PWM0
#define PWM_PORT_PWM1	HI_PWM_PORT_PWM1

#endif /* __CAR_TEST_H__ */
