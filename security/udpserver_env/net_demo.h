
#ifndef NET_DEMO_COMMON_H
#define NET_DEMO_COMMON_H

#include <stdio.h>

void NetDemoTest (unsigned short port, const char *host);

const char *GetNetDemoName (void);

#define IMPL_GET_NET_DEMO_NAME(testFunc) \
	const char* GetNetDemoName() { \
		static const char* demoName = #testFunc; \
		return demoName; \
	}

#define CLIENT_TEST_DEMO(testFunc) \
	void NetDemoTest(unsigned short port, const char* host) { \
		(void) host; \
		printf("%s start\r\n", #testFunc); \
		printf("I will connect to %s:%d\r\n", host, port); \
		testFunc(host, port); \
		printf("%s done!\r\n", #testFunc); \
	} \
	IMPL_GET_NET_DEMO_NAME(testFunc)

#define SERVER_TEST_DEMO(testFunc) \
	void NetDemoTest(unsigned short port, const char* host) { \
		(void) host; \
		printf("%s start\r\n", #testFunc); \
		printf("I will listen on :%d\r\n", port); \
		testFunc(port); \
		printf("%s done!\r\n", #testFunc); \
	} \
	IMPL_GET_NET_DEMO_NAME(testFunc)

#endif // NET_DEMO_COMMON_H
