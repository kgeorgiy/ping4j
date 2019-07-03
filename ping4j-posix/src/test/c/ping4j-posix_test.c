#include "ping4j-posix.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

void print4(PING4J_IPV4_ADDRESS* address) {
    printf(
        "%d.%d.%d.%d\n",
         address->octets[0],
         address->octets[1],
         address->octets[2],
         address->octets[3]
    );
}

void print4v(void* address) {
    print4((PING4J_IPV4_ADDRESS*) address);
}

uint16_t word6(PING4J_IPV6_ADDRESS* address, int no) {
    return ((uint16_t) address->octets[no * 2] << 8) + address->octets[no * 2 + 1];
}

void print6(PING4J_IPV6_ADDRESS* address) {
    printf(
        "%x:%x:%x:%x:%x:%x:%x:%x\n",
        word6(address, 0),
        word6(address, 1),
        word6(address, 2),
        word6(address, 3),
        word6(address, 4),
        word6(address, 5),
        word6(address, 6),
        word6(address, 7)
    );
}


void print6v(void* address) {
    print6((PING4J_IPV6_ADDRESS*) address);
}

struct DnsSettings {
    const char* type;
    int family;
    size_t offset;
    size_t size;
    void (*print)(void *);
};

static struct DnsSettings dns4Settings = {
    .type = "A",
    .family = AF_INET,
    .offset = offsetof(struct sockaddr_in, sin_addr),
    .size = 4,
    .print = (void *) &print4v
};

static struct DnsSettings dns6Settings = {
    .type = "AAAA",
    .family = AF_INET6,
    .offset = offsetof(struct sockaddr_in6, sin6_addr),
    .size = 16,
    .print = (void *) &print6v
};

void dns(const char* name, struct DnsSettings* settings, void* address) {
    printf("DNS type %s query for %s\n", settings->type, name);
    memset(address, 0, settings->size);

    struct addrinfo hints = {.ai_family = settings->family, .ai_socktype = SOCK_DGRAM};

    struct addrinfo* result;
    int s = getaddrinfo(name, NULL, &hints, &result);
    if (s != 0) {
        printf("\tDNS error %d %d\n", s, EAI_NONAME);
        return;
    }

    bool found = false;
    for (struct addrinfo* p = result; p != NULL; p = p->ai_next) {
        if (p->ai_family == settings->family) {
            found = true;
            memcpy(address, ((char *) p->ai_addr) + settings->offset, settings->size);
            printf("\t%s: ", name);
            settings->print(address);
        }
    }
    freeaddrinfo(result);

    assert(found);
}

PING4J_IPV4_ADDRESS dns4(const char* name) {
    PING4J_IPV4_ADDRESS address;
    dns(name, &dns4Settings, &address.octets);
    return address;
}

PING4J_IPV6_ADDRESS dns6(const char* name) {
    PING4J_IPV6_ADDRESS address;
    dns(name, &dns6Settings, &address.octets);
    return address;
}

uint32_t asserts = 0;
uint32_t success = 0;
uint32_t failure = 0;

const char* RESULT_SUCCESS_STR  = "SUCCESS";
const char* RESULT_ERROR_STR    = "ERROR";
const char* RESULT_STATUS_STR   = "STATUS";

const char* resultStr(uint32_t result) {
    if (result == RESULT_SUCCESS) {
        return RESULT_SUCCESS_STR;
    } else if (result == RESULT_ERROR) {
        return RESULT_ERROR_STR;
    } else if (result == RESULT_STATUS) {
        return RESULT_STATUS_STR;
    }
    assert(false);
}

void assertResult(uint32_t expectedResult, struct Ping4jResult* result) {
    printf("\tresult=%d, value=%d\n", result->result, result->value);
    asserts++;
    if (result->result == expectedResult) {
        success++;
    } else {
        failure++;
        printf(
            "\tFAIL: expected %s (%d), got %s (%d):\n",
            resultStr(expectedResult), expectedResult,
            resultStr(result->result), result->result
        );
    }
}

void check4(uint32_t expectedResult, PING4J_IPV4_ADDRESS* address, uint32_t timeout, uint8_t ttl, uint16_t size) {
    printf("ping4 ");
    print4(address);
    printf("\ttimeout=%d, ttl=%d, size=%d\n", timeout, ttl, size);

    struct Ping4jResult result;
    ping4jPing4(address, timeout, ttl, size, &result);
    assertResult(expectedResult, &result);
}

void checkSuccess4(PING4J_IPV4_ADDRESS* address, bool global) {
    check4(RESULT_SUCCESS, address, 10000, 100, 100);
    check4(RESULT_SUCCESS, address, 10000, 100, 1);

    if (global){
        check4(RESULT_STATUS, address, 1, 100, 100);
        check4(RESULT_STATUS, address, 100, 1, 100);
    }
}

void checkFail4(PING4J_IPV4_ADDRESS* address) {
    check4(RESULT_STATUS, address, 100, 100, 100);
}

void check6(uint32_t expectedResult, PING4J_IPV6_ADDRESS* address, uint32_t timeout, uint8_t ttl, uint16_t size) {
    printf("ping6 ");
    print6(address);
    printf("\ttimeout=%d, ttl=%d, size=%d\n", timeout, ttl, size);

    struct Ping4jResult result;
    ping4jPing6(address, timeout, ttl, size, &result);
    assertResult(expectedResult, &result);
}

void checkSuccess6(PING4J_IPV6_ADDRESS* address, bool global) {
    check6(RESULT_SUCCESS, address, 10000, 100, 100);
    check6(RESULT_SUCCESS, address, 10000, 100, 1);

    if (global){
        check6(RESULT_STATUS, address, 1, 100, 100);
        check6(RESULT_STATUS, address, 100, 1, 100);
    }
}

void checkFail6(PING4J_IPV6_ADDRESS* address) {
    check6(RESULT_STATUS, address, 100, 100, 100);
}

#pragma GCC diagnostic ignored "-Wunused-variable"
int main() {
    PING4J_IPV4_ADDRESS local4 = {{127, 0, 0, 1}};
    PING4J_IPV4_ADDRESS reserved4 = {{192, 0, 1, 1}};
    PING4J_IPV4_ADDRESS localhost4 = dns4("localhost");
    PING4J_IPV4_ADDRESS googleDns4 = {{8, 8, 8, 8}};
    PING4J_IPV4_ADDRESS google4 = dns4("ipv4.google.com");

    PING4J_IPV6_ADDRESS local6 = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS reserved6 = {{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS localhost6 = dns6("ip6-localhost");
    PING4J_IPV6_ADDRESS google6 = dns6("ipv6.google.com");

    ping4jInit();

    checkSuccess4(&local4, false);
    checkFail4(&reserved4);
    checkSuccess4(&localhost4, false);
    checkSuccess4(&googleDns4, true);
    checkSuccess4(&google4, true);

//    checkSuccess6(&local6, false);
//    checkFail6(&reserved6);
//    checkSuccess6(&localhost6, false);
//    checkSuccess6(&google6, true);

    if (failure == 0) {
        printf("SUCCESS (%d tests)\n", asserts);
        return 0;
    } else {
        printf("FAILED %d of %d tests\n", failure, asserts);
        return 1;
    }
}
