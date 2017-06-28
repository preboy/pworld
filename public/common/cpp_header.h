#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (PLAT_WIN32)
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#else
#include <strings.h>
#include <dirent.h>
#include <stdint.h>
#include <error.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <errno.h>



#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <thread>
#include <mutex>
#include <ctime>
#include <random>
#include <atomic>
