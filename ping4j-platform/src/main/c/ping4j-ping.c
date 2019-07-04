#include "ping4j.h"

#include <assert.h>

void ping4jPing(
    const PING4J_IP_ADDRESS* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    PING4J_RESULT* const result
) {
    if (address->version == PING4J_IPV4_VERSION) {
        ping4jPing4(&address->u.ipv4, timeout, ttl, packetSize, result);
    } else if (address->version == PING4J_IPV6_VERSION) {
        ping4jPing6(&address->u.ipv6, timeout, ttl, packetSize, result);
    } else {
        assert(0);
    }
}
