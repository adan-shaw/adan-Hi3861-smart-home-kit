
#ifndef NET_COMMON_H
#define NET_COMMON_H

// __arm__ and __aarch64__ for HarmonyOS with liteos-a kernel, __i386__ and __x86_64__ for Unix like OS
#if defined(__arm__) || defined(__aarch64__) || defined(__i386__) || defined(__x86_64__)
#define HAVE_BSD_SOCKET 1
#else
#define HAVE_BSD_SOCKET 0
#endif

#if defined(__riscv)						// for wifiiot(HarmonyOS on Hi3861 with liteos-m kernel)
#define HAVE_LWIP_SOCKET 1
#else
#define HAVE_LWIP_SOCKET 0
#endif

#if HAVE_BSD_SOCKET
#include <sys/types.h>					// for AF_INET SOCK_STREAM
#include <sys/socket.h>					// for socket
#include <netinet/in.h>					// for sockaddr_in
#include <arpa/inet.h>					// for inet_pton
#elif HAVE_LWIP_SOCKET
#include "lwip/sockets.h"
#ifndef close
#define close(fd) lwip_close(fd)
#endif
#else
#error "Unknow platform!"
#endif

#endif // NET_COMMON_H
