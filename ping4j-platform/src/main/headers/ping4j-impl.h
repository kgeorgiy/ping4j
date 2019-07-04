#ifndef PING4J_IMPL_H
#define PING4J_IMPL_H

#include "ping4j.h"

#ifdef __cplusplus
extern "C" {
#endif
void ping4jDns4Impl(
    const char* const name,
    void* const addresses,
    const size_t maxAddresses,
    const size_t addressIncrement,
    PING4J_RESULT* const result
);

void ping4jDns6Impl(
    const char* const name,
    void* const addresses,
    const size_t maxAddresses,
    const size_t addressIncrement,
    PING4J_RESULT* const result
);
#ifdef __cplusplus
}
#endif

#endif
