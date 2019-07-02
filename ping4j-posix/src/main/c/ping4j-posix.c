#include "ping4j-posix.h"

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <errno.h>

#include <stdbool.h>

static unsigned char DATA[1 << 16];

void ping4jInit() {
    for (unsigned int i = 0; i < sizeof(DATA); i++) {
        DATA[i] = (unsigned char) i;
    }
}

bool setResult(struct Ping4jResult* pingResult, uint32_t result, uint32_t value) {
    pingResult->result = result;
    pingResult->value = value;
    return false;
}

//static const int REPLY_SIZE = 65530;

struct Request {
    int sock;
    uint8_t reply[65530];
};

void ping4jPing4(
    struct Ping4jIpv4Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        setResult(result, RESULT_ERROR, errno);
    }

    setResult(result, RESULT_SUCCESS, 0);
}

void ping4jPing6(
    struct Ping4jIpv6Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    setResult(result, RESULT_SUCCESS, 0);
}
