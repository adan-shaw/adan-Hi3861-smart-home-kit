**摘要**: 本文简单介绍如何在OpenHarmony中增加软件包的功能

**适合群体**: 适用于润和Hi3861开发板

文中所有代码仓库: https://gitee.com/qidiyun/hihope-3861-smart-home-kit 

## 10.1 添加第一个a_myparty软件包
打开鸿蒙系统的源码, 可以看到有这么一个文件夹: third_party;里面存放的是第三方的代码;
![image.png](https://img-blog.csdnimg.cn/img_convert/494399a02c3e4a0d81df37b3bee54fff.png)
点开我们可以看到有很多第三方代码: 
![image.png](https://img-blog.csdnimg.cn/img_convert/eb2d2335d92ec72691d0c45781108b88.png)

后续我们如果需要往系统中添加、移植任何开源代码, 都可以添加到这个文件夹中;接下来, 教大家如何添加一个自己的软件包, 名字为a_myparty;

**1. 新建一个文件夹a_myparty**

**2. 往文件中放置软件包源码**
这里我放在的是 myparty.c和myparty.h文件

**3. 新建BUILD.gn文件**
整个代码目录如下: 
![image.png](https://img-blog.csdnimg.cn/img_convert/a406996a729044461b3ede9880cdc596.png)
4. myparty.c文件内容如下: 
其实, 我这个只是为了演示的, 所以里面代码没什么作用
```
#include <stdio.h>

void myparty_test(void)
{
	printf("first myparty \r\n");
}

```

**5. BUILD.gn文件内容如下: **

BUILD.gn文件主要是描述了软件包的相关信息, 包括编译哪些源文件, 头文件路径、编译方式(目前Hi3861 只支持静态加载)
```
import("//build/lite/config/component/lite_component.gni")
import("//build/lite/ndk/ndk.gni")

#这里是配置头文件路径
config("a_myparty_config") {
	include_dirs = [
		".",
	]
}

#这里是配置要编译哪些源码
a_myparty_sources = [
	"myparty.c",
]

#这里是静态链接, 类似于Linux系统的 .a文件
lite_library("a_myparty_static") {
	target_type = "static_library"
	sources = a_myparty_sources
	public_configs = [ ":a_myparty_config" ]

}

#这里是动态加载, 类似于Linux系统的 .so文件
lite_library("a_myparty_shared") {
	target_type = "shared_library"
	sources = a_myparty_sources
	public_configs = [ ":a_myparty_config" ]

}

#这里是入口, 选择是静态还是动态
ndk_lib("a_myparty_ndk") {

	if (kernel_type != "liteos_m") {
		lib_extension = ".so"
		deps = [
			":a_myparty_shared"
		]
	} else {
		deps = [
			":a_myparty_static"
		]
	}
	head_files = [
		"//third_party/a_myparty"
	]

}
```


到了这里我们基本上就写完了;
最后我们要让这个第3放软件包编译到我们固件中;

## 10.2 如何使用a_myparty软件包
我们在app里面新建一个 myparty_demo的文件夹, 目录如下: 
![image.png](https://img-blog.csdnimg.cn/img_convert/a4d29e8f0d2924581febb5e64d1b9fec.png)
其中BUILD.gn内容如下: 
```
static_library("myparty_demo") {
	sources = [
		"myparty_demo.c"
	]
#注意需要把a_myparty的头文件路径加进来
	include_dirs = [
		"//utils/native/lite/include",
		"//third_party/a_myparty",
	]

#表示需要a_myparty 软件包
	deps = [
		"//third_party/a_myparty:a_myparty_static",
	]
}
```



myparty_demo.c里面内容如下
```
#include <stdio.h>
#include "ohos_init.h"
#include "ohos_types.h"

#include "myparty.h"

void myparty_demo(void)
{
	myparty_test();
	printf("___________>>>>>>>>>>>>>>>>>>>> [DEMO] Hello world.\n");
}
SYS_RUN(myparty_demo);
```


编译烧录后可以看到myparty_test的打印信息: 
![image.png](https://img-blog.csdnimg.cn/img_convert/01cf2dbdd629a28338771bc6580cb6d7.png)
