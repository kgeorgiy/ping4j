#ifndef PING4J_POSIX_H
#define PING4J_POSIX_H

#include <stdint.h>

static const uint32_t RESULT_SUCCESS = 0;
static const uint32_t RESULT_ERROR = 1;
static const uint32_t RESULT_STATUS = 2;

typedef struct Ping4jIpv4Address {
    uint8_t octets[4];
} PING4J_IPV4_ADDRESS;

typedef struct Ping4jIpv6Address {
    uint8_t octets[16];
} PING4J_IPV6_ADDRESS;

typedef struct Ping4jResult {
    uint32_t result;
    uint32_t value;
} PING4J_RESULT;

#ifdef __cplusplus
extern "C" {
#endif
void ping4jInit();

void ping4jPing4(
    const struct Ping4jIpv4Address* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    struct Ping4jResult* result
);

void ping4jPing6(
    const struct Ping4jIpv6Address* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    struct Ping4jResult* result
);
#ifdef __cplusplus
}
#endif

#endif
