#include "ping4j-impl.h"

void ping4jDns4(
    const char* const name,
    PING4J_IPV4_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
) {
    ping4jDns4Impl(name, addresses, maxAddresses, sizeof(PING4J_IPV4_ADDRESS), result);
}

void ping4jDns6(
    const char* const name,
    PING4J_IPV6_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
) {
    ping4jDns6Impl(name, addresses, maxAddresses, sizeof(PING4J_IPV6_ADDRESS), result);
}

void ping4jDns(
    const char* const name,
    PING4J_IP_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
) {
    ping4jDns4Impl(name, &addresses->u, maxAddresses, sizeof(PING4J_IP_ADDRESS), result);
    if (result->result != RESULT_SUCCESS) {
        return;
    }

    uint32_t v4 = result->value;
    for (uint32_t i = 0; i < result->value; i++) {
        addresses[i].version = PING4J_IPV4_VERSION;
    }

    if (v4 == maxAddresses) {
        return;
    }

    ping4jDns6Impl(name, &(addresses + v4)->u, maxAddresses - v4, sizeof(PING4J_IP_ADDRESS), result);
    if (result->result != RESULT_SUCCESS) {
        return;
    }
    for (uint32_t i = 0; i < result->value; i++) {
        addresses[i + v4].version = PING4J_IPV6_VERSION;
    }
    result->value += v4;
}
