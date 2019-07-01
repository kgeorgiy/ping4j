#ifndef PING4J_WINDOWS_H
#define PING4J_WINDOWS_H

#include <stdint.h>

static const uint32_t RESULT_SUCCESS = 0;
static const uint32_t RESULT_LAST_ERROR = 1;
static const uint32_t RESULT_STATUS = 2;

typedef struct Ping4jIpv4Address {
    uint8_t octets[4];
} PING4J_IPV4_ADDRESS;

typedef struct Ping4jIpv6Address {
    uint8_t octets[16];
} PING4J_IPV6_ADDRESS;

struct Ping4jResult {
    uint32_t result;
    uint32_t value;
} PING4J_RESULT;

#ifdef __cplusplus
extern "C" {
#endif
void ping4jInit();

__declspec(dllexport)
void ping4jPing4(
    struct Ping4jIpv4Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
);

__declspec(dllexport)
void ping4jPing6(
    struct Ping4jIpv6Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
);
#ifdef __cplusplus
}
#endif

#endif
