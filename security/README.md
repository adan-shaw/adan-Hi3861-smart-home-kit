# 润和Pegasus套件样例--智能安防
该样例展示OpenHarmony智能安防项目, 当温度传感器超过设定值后, 或者烟雾传感器检测到烟雾时, 会触发蜂鸣器工作、同时通知到HarmonyOS手机上的APP;
![在这里插入图片描述](https://img-blog.csdnimg.cn/e299e84de5fb478187035f286b440a12.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6L-e5b-X5a6J55qE5Y2a5a6i,size_11,color_FFFFFF,t_70,g_se,x_16)

## 下载源码

建议将本教程的设备源码下载后, 放在applications/sample/wifi-iot/app目录下: 

* 执行命令`git clone https://gitee.com/hihope_iot/hispark-pegasus-sample/tree/master/30_samart_environmental_monitoring_system`

* APP源码下载: 

  `git clone https://gitee.com/hihopeorg_group/hcia_harmonyos_application`



## 文件说明

udpserver_env目录说明: 

| 文件名			 | 说明									  |
| ------------------ | ---------------------------------------------- |
| BUILD.gn	  | HarmonyOS 构建脚本							 |
| cjson.c			| json格式数据解析、封装						 |
| config_params.h	| 业务参数配置							  |
| demo_entry_cmsis.c | HarmonyOS liteos-m程序入口					 |
| net_common.h  | 系统网络接口头文件							 |
| net_demo.h		 | demo脚手架头文件						  |
| net_params.h  | 网络参数, 包括WiFi热点信息, 服务器IP、端口信息 |
| udp_server_test.c  | UDP服务端								 |
| wifi_connecter.c   | HarmonyOS WiFi STA模式API的封装实现文件		|
| wifi_connecter.h   | HarmonyOS WiFi STA模式API的封装头文件	 |
| wifi_starter.c	 | HarmonyOS WiFi AP模式API的封装实现文件		 |
| wifi_starter.h	 | HarmonyOS WiFi AP模式API的封装头文件	  |

## 实验指南

### 准备网络环境

1. 准备一个无线路由器, 
2. harmony os 手机一部

### 编译和烧录测试程序

在Hi3861开发板上运行程序, 需要分别编译、烧录程序;

1. 需要修改`applications/sample/wifi-iot/app/BUILD.gn`代码, 注释掉不需要运行的代码: 

   添加`"environment:sensing_demo",`

   ​  ` "udpserver_env:net_demo"`

2. 使用DevEco Device Tool 进行一键式编译或者执行`hb build -f`

3. 使用DevEco Device Tool或者HiBurn将二进制程序烧录到Hi3861开发板上;

4. 烧录成功后请复位程序

### 测试APP安装

将提供的harmony os APP 安装到手机上

### 运行测试程序

1. 复位开发板上的程序后, 手机连接上开发板的热点`HarmonyOS-AP`,然后打开安装好的APP, 配置新的热点名称与密码, 点击`配网`按键

   将热点的名称与密码下发到板端, 然后关闭APP

2. 板端接收到APP下发的热点与密码之后会重启设备, 并连接上配置好的热点, 手机同样连接上配置的热点, 再次打开APP;板端与手机APP建立通信

3. APP切换到环境监测界面, 在1秒之后会自动更新温湿度以及可燃气体浓度的值, 之后会每隔1分钟更新一次数据, 还可以点击`Update`进行数据更新;

4. 环境监测界面可以修改需要监测的数值范围, 在超出设置的范围时, 蜂鸣器会响, 进行报警;
