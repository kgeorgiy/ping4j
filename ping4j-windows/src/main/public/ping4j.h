#ifndef PING4J_H
#define PING4J_H

#if defined(_WIN32)
    #define EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #pragma error Unknown dynamic link import/export semantics.
#endif

#include <stdint.h>

static const uint32_t RESULT_SUCCESS = 0;
static const uint32_t RESULT_ERROR = 1;
static const uint32_t RESULT_STATUS = 2;

#define PING4J_IPV4_ADDRESS_SIZE 4
#define PING4J_IPV4_VERSION 4
typedef struct Ping4jIpv4Address {
    uint8_t octets[PING4J_IPV4_ADDRESS_SIZE];
} PING4J_IPV4_ADDRESS;

#define PING4J_IPV6_ADDRESS_SIZE 16
#define PING4J_IPV6_VERSION 6
typedef struct Ping4jIpv6Address {
    uint8_t octets[PING4J_IPV6_ADDRESS_SIZE];
} PING4J_IPV6_ADDRESS;

typedef struct Ping4jIpAddress {
    uint8_t version;
    union {
        PING4J_IPV4_ADDRESS ipv4;
        PING4J_IPV6_ADDRESS ipv6;
    } u;
} PING4J_IP_ADDRESS;

typedef struct Ping4jResult {
    uint32_t result;
    uint32_t value;
} PING4J_RESULT;

#ifdef __cplusplus
extern "C" {
#endif
EXPORT void ping4jInit();

EXPORT void ping4jPing4(
    const PING4J_IPV4_ADDRESS* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    PING4J_RESULT* const result
);

EXPORT void ping4jPing6(
    const PING4J_IPV6_ADDRESS* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    PING4J_RESULT* const result
);

EXPORT void ping4jPing(
    const PING4J_IP_ADDRESS* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    PING4J_RESULT* const result
);

EXPORT void ping4jDns4(
    const char* const name,
    PING4J_IPV4_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
);

EXPORT void ping4jDns6(
    const char* const name,
    PING4J_IPV6_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
);

EXPORT void ping4jDns(
    const char* const name,
    PING4J_IP_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
);

#ifdef __cplusplus
}
#endif

#endif
