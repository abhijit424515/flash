#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>
#include <functional>
#include <stdexcept>
#include <atomic>
#include <condition_variable>
#include <mutex>

#define PORT 6378
#define BACKLOG 256
#define DEFAULT_BUFFER_SIZE 4096

using namespace std;