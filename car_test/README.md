**摘要**: 本文简单介绍鸿蒙系统 + Hi3861 的WiFi小车开发

**适合群体**: 适用于润和Hi3861开发板

文中所有代码仓库: https://gitee.com/qidiyun/hihope-3861-smart-home-kit 

 

## 13.1 小车介绍

基于鸿蒙系统 + Hi3861 的WiFi小车

首先, 我们得有一套WiFi小车套件, 其实也是Hi3861 加上电机、循迹模块、超声波等模块;

小车安装完大概是这样: 
![在这里插入图片描述](https://img-blog.csdnimg.cn/3e2bf6202ffa4d2a882cece10c5ed814.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_20,color_FFFFFF,t_70,g_se,x_16)

 

## 13.2 电机驱动

我们这里先只做最简单的, 驱动小车的电机, 让小车跑起来;

电机的驱动板如下图, 目前电机驱动芯片用的是L9110S芯片;

![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/2ca39f8f19c141fea6b9138935df4565.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_11,color_FFFFFF,t_70,g_se,x_16)

典型的应用电路如下图: 
![在这里插入图片描述](https://img-blog.csdnimg.cn/f7a3914d218348a1af7f9ba11b5539d9.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_14,color_FFFFFF,t_70,g_se,x_16)



   我们可以看到, 如果要控制电机, 我们芯片至少需要2路PWM信号, 一路用于控制正转, 一路用于控制反转;

然后我们小车有两个轮子, 需要两个电机, 所以我们需要 4 路PWM信号;

查阅小车资料, 可以知道, 目前Hi3861 芯片用来控制电机的4路PWM分别是: 

 ![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/0ae2da75b6b5443fa39503b175a35b1b.png)

知道了PWM通道和对应的GPIO口, 我们就可以开始编程了;

 

首先PWM初始化部分: 

```c
void pwm_init(void)
{
	hi_gpio_init();
	//引脚复用
	hi_io_set_func(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_PWM3_OUT); 
	hi_io_set_func(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_PWM4_OUT); 	
	hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_PWM0_OUT); 
	hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_PWM1_OUT); 

	//初始化pwm
	hi_pwm_init(HI_PWM_PORT_PWM3);
	hi_pwm_init(HI_PWM_PORT_PWM4);
	hi_pwm_init(HI_PWM_PORT_PWM0);
	hi_pwm_init(HI_PWM_PORT_PWM1);

}
```

 

控制小车前进、后退、左转、右转、停止的函数: 

```c
//停止
void pwm_stop(void)
{
	//先停止PWM
	hi_pwm_stop(HI_PWM_PORT_PWM3);
	hi_pwm_stop(HI_PWM_PORT_PWM4);
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	hi_pwm_stop(HI_PWM_PORT_PWM1);
}
void car_stop(void)
{
	car_info.cur_status = car_info.go_status;

	printf("pwm_stop \r\n");

	pwm_stop();
}

//前进
void pwm_forward(void)
{
	//先停止PWM
	hi_pwm_stop(HI_PWM_PORT_PWM3);
	hi_pwm_stop(HI_PWM_PORT_PWM4);
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	hi_pwm_stop(HI_PWM_PORT_PWM1);

	//启动A路PWM
	hi_pwm_start(HI_PWM_PORT_PWM3, 750, 1500);
	hi_pwm_start(HI_PWM_PORT_PWM0, 750, 1500);
}
void car_forward(void)
{
	if(car_info.go_status != CAR_STATUS_FORWARD)
	{
		//直接退出
		return ;
	}
	if(car_info.cur_status == car_info.go_status)
	{
		//状态没有变化, 直接推出
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf("pwm_forward \r\n");

	pwm_forward();

	step_count_update();
}

//后退
void pwm_backward(void)
{
	//先停止PWM
	hi_pwm_stop(HI_PWM_PORT_PWM3);
	hi_pwm_stop(HI_PWM_PORT_PWM4);
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	hi_pwm_stop(HI_PWM_PORT_PWM1);

	//启动A路PWM
	hi_pwm_start(HI_PWM_PORT_PWM4, 750, 1500);
	hi_pwm_start(HI_PWM_PORT_PWM1, 750, 1500);
}
void car_backward(void)
{
	if(car_info.go_status != CAR_STATUS_BACKWARD)
	{
		//直接退出
		return ;
	}
	if(car_info.cur_status == car_info.go_status)
	{
		//状态没有变化, 直接推出
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf("pwm_backward \r\n");

	pwm_backward();

	step_count_update();
}

//左转
void pwm_left(void)
{
	//先停止PWM
	hi_pwm_stop(HI_PWM_PORT_PWM3);
	hi_pwm_stop(HI_PWM_PORT_PWM4);
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	hi_pwm_stop(HI_PWM_PORT_PWM1);

	//启动A路PWM
	hi_pwm_start(HI_PWM_PORT_PWM3, 750, 1500);
}
void car_left(void)
{
	if(car_info.go_status != CAR_STATUS_LEFT)
	{
		//直接退出
		return ;
	}
	if(car_info.cur_status == car_info.go_status)
	{
		//状态没有变化, 直接推出
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf("pwm_left \r\n");

	pwm_left();

	step_count_update();
}

//右转
void pwm_right(void)
{
	//先停止PWM
	hi_pwm_stop(HI_PWM_PORT_PWM3);
	hi_pwm_stop(HI_PWM_PORT_PWM4);
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	hi_pwm_stop(HI_PWM_PORT_PWM1);

	//启动A路PWM
	hi_pwm_start(HI_PWM_PORT_PWM0, 750, 1500);
}
void car_right(void)
{
	if(car_info.go_status != CAR_STATUS_RIGHT)
	{
		//直接退出
		return ;
	}
	if(car_info.cur_status == car_info.go_status)
	{
		//状态没有变化, 直接推出
		return;
	}

	car_info.cur_status = car_info.go_status;

	printf("pwm_right \r\n");

	pwm_right();

	step_count_update();
}
```


最后, 要使用pwm功能, 我们需要修改 

device/soc/hisilicon/hi3861v100/sdk_liteos/build/config/usr_config.mk

增加这两行, 这里是打开PWM功能

```c
CONFIG_PWM_SUPPORT=y

CONFIG_PWM_HOLD_AFTER_REBOOT=y
```

![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/53a465da64cf4c90971d92c57789574b.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_17,color_FFFFFF,t_70,g_se,x_16)


 

13.3 WiFi控制部分

 

我们在小车上面简单编写一个UDP程序, 监听50001端口号;这里使用的通信格式是json, 小车收到UDP数据后, 解析json, 并根据命令执行相应的操作, 例如前进、后退、左转、右转等, 代码如下: 

![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/ea79217294864726af8c4025f8ff4161.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_20,color_FFFFFF,t_70,g_se,x_16)


 

电脑端, 使用C#编写一个测试程序, 可以手动输入小车的IP地址, 也可以不输入IP地址, 这样, 电脑端程序会发送广播包给小车, 也可以起到控制的功能;

 ![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/fe30ad601b964a43875e2786eff9bf1a.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_16,color_FFFFFF,t_70,g_se,x_16)


 

13.4 WiFi热点连接

注意, 我这里WiFi小车固件默认连接热点的ssid是: 

 ![OpenHarmony轻量系统开发[13]鸿蒙小车开发-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/b9a663380c434f9ea5420d487e8eb9b5.png)


读者需要自己修改成自己的热点;

文件: sta_entry.c

小车的源码, C#控制端的代码均开源, 大家可以自由修改, 发挥自己的想象, 创造出更厉害炫酷的DIY产品;
