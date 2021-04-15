#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_client.h"
#include "net_server.h"
#include "net_connection.h"
#include "net_message_type.h"


#define DEFAULT_SERVER_PORT 60000
#define DEFAULT_TICK 10000

extern const std::string DEFAULT_SERVER_HOST = "127.0.0.1";
extern const std::string CONFIG_FILE = "../config.txt";
