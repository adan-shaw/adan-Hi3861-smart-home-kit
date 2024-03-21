**摘要**: 本文简单介绍如何操作ADC去读取电压, 并且实现开发板上3个ADC按键检测的功能

**适合群体**: 适用于润和Hi3861开发板, L0轻量系统驱动开发

文中所有代码仓库: https://gitee.com/qidiyun/hihope-3861-smart-home-kit 


## 6.1实验效果

查看开发板, 可以看到除了复位按键之外, 还有3个按键;而查看原理, 我们可以看到这个3个按键其实都是接的GPIO5 引脚, 而GPIO5引脚又可复用为ADC2引脚;

![OpenHarmony轻量系统开发[6]驱动之ADC按键-鸿蒙HarmonyOS技术社区](https://img-blog.csdnimg.cn/65b5910ff96b405db0d5115f5398278b.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_20,color_FFFFFF,t_70,g_se,x_16)


故而, 我们可以猜测出来我们可以使用ADC检测电压, 判断出来是哪个引脚被按下了;

看下效果: 

当我按下 按键1的时候, 串口会打印: 

![在这里插入图片描述](https://img-blog.csdnimg.cn/77610e6deded4782a87e81bbefb078a5.png)


当我按下按键2的时候串口会打印: 
![在这里插入图片描述](https://img-blog.csdnimg.cn/54d1123f4a2749a0a4e57c5ca1ad6437.png)

 

当我按下 USER按键的时候串口会打印

![在这里插入图片描述](https://img-blog.csdnimg.cn/bd221c42999b43ac9f7bbbdb88f8c54c.png)


 

其中 vlt_min 表示读取到ADC值的最小值, 

vlt_max 表示读取到ADC值的最大值;

由此我们可以看到, 按键1被按下的时候, ADC值得范围在 0.563 ~ 0.577 

按键2按下后, ADC值在 0.963 ~ 0.970 

USER按键按下后 ADC值 在 0.197 ~ 0.204 

 

如果没有按键按下, 则ADC值在 3.227 ~ 3.241 

vlt_min:3.227, vlt_max:3.241 

 

## 6.2代码实现

代码实现其实很简单;

### (1)引脚初始化

这里由于GPIO5默认被复用为串口引脚, 这里我们重新修改为普通GPIO引脚;初始化代码如下: 

	(hi_void)hi_gpio_init();
	
	hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO); /* uart1 rx */

	ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
	if (ret != HI_ERR_SUCCESS) {
		printf("===== ERROR ======gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
		return;
	}


###(2)读取ADC值

读取ADC值的代码页相对简单, 这里, 我是重复读取64次, 减少误判;

	memset_s(g_adc_buf, sizeof(g_adc_buf), 0x0, sizeof(g_adc_buf));
 
	for (i = 0; i < ADC_TEST_LENGTH; i++) {
		ret = hi_adc_read((hi_adc_channel_index)HI_ADC_CHANNEL_2, &data, HI_ADC_EQU_MODEL_1, HI_ADC_CUR_BAIS_DEFAULT, 0);
		if (ret != HI_ERR_SUCCESS) {
			printf("ADC Read Fail\n");
			return;
		}
		g_adc_buf[i] = data;
	}


###(3)对读出来的ADC值进行判断处理

 S1对应的是按键1 、 S2对应的是按键2 、 S3对应的是 USER按键

	for (i = 0; i < data_len; i++) {
		vlt = g_adc_buf[i];
		float voltage = (float)vlt * 1.8 * 4 / 4096.0;  /* vlt * 1.8 * 4 / 4096.0: Convert code into voltage */
		vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
		vlt_min = (voltage < vlt_min) ? voltage : vlt_min;
	}
	//printf("vlt_min:%.3f, vlt_max:%.3f \n", vlt_min, vlt_max);

	vlt_val = (vlt_min + vlt_max)/2.0;

	if((vlt_val > 0.4) && (vlt_val < 0.6))
	{
		if(key_flg == 0)
		{
			key_flg = 1;
			key_status = KEY_EVENT_S1;
		}
	}
	if((vlt_val > 0.8) && (vlt_val < 1.1))
	{
		if(key_flg == 0)
		{
			key_flg = 1;
			key_status = KEY_EVENT_S2;
		}
	}

	if((vlt_val > 0.01) && (vlt_val < 0.3))
	{
		if(key_flg == 0)
		{
			key_flg = 1;
			key_status = KEY_EVENT_S3;
		}
	}

	if(vlt_val > 3.0)
	{
		key_flg = 0;
		key_status = KEY_EVENT_NONE;
	}


###(4)使用

编写好上面代码后, 就可以直接在while循环中判断按键值了: 

	
	while(1)
	{
		//读取ADC值
		app_demo_adc_test();

		switch(get_key_event())
		{
			case KEY_EVENT_NONE:
			{
				
			}
			break;

			case KEY_EVENT_S1:
			{
				printf("KEY_EVENT_S1 \r\n");
			}
			break;

			case KEY_EVENT_S2:
			{
				printf("KEY_EVENT_S2 \r\n");
			}
			break;

			case KEY_EVENT_S3:
			{
				printf("KEY_EVENT_S3 \r\n");
			}
			break;

		}

		usleep(30000);
	}
	
