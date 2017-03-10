#pragma once

#include <stdio.h>
#include <stdlib.h>

#if defined (PLAT_WIN)
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#else
#include <stdint.h>
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

