#include "ping4j-impl.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

static void setResult(PING4J_RESULT* const pingResult, uint32_t result, uint32_t value) {
    pingResult->result = result;
    pingResult->value = value;
}

static void dnsImpl(
    const char* const name,
    void* const addresses,
    const size_t maxAddresses,
    const size_t addressIncrement,
    PING4J_RESULT* const result,
    const int family,
    const size_t addressSize,
    const size_t addressOffset
) {
    struct addrinfo hints = {.ai_family = family, .ai_socktype = SOCK_DGRAM};

    struct addrinfo* info;
    int err = getaddrinfo(name, NULL, &hints, &info);

    if (err != 0) {
        setResult(result, RESULT_ERROR, err);
        return;
    }

    uint8_t* address = (uint8_t*) addresses;
    uint32_t found = 0;
    for (struct addrinfo* p = info; p != NULL; p = p->ai_next) {
        if (p->ai_family == family) {
            memcpy(address, ((uint8_t*) p->ai_addr) + addressOffset, addressSize);
            address += addressIncrement;
            found++;
        }
    }

    freeaddrinfo(info);
    setResult(result, RESULT_SUCCESS, found);
}

void ping4jDns4Impl(
    const char* const name,
    void* const addresses,
    const size_t maxAddresses,
    const size_t addressIncrement,
    PING4J_RESULT* const result
) {
    dnsImpl(
        name, addresses, maxAddresses, addressIncrement, result,
        AF_INET, PING4J_IPV4_ADDRESS_SIZE, offsetof(struct sockaddr_in, sin_addr)
    );
}

void ping4jDns6Impl(
    const char* const name,
    void* const addresses,
    const size_t maxAddresses,
    const size_t addressIncrement,
    PING4J_RESULT* const result
) {
    dnsImpl(
        name, addresses, maxAddresses, addressIncrement, result,
        AF_INET6, PING4J_IPV6_ADDRESS_SIZE, offsetof(struct sockaddr_in6, sin6_addr)
    );
}
