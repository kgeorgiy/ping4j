#include "ping4j.h"
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

uint32_t asserts = 0;
uint32_t success = 0;
uint32_t failure = 0;

const char* RESULT_SUCCESS_STR  = "SUCCESS";
const char* RESULT_ERROR_STR    = "ERROR";
const char* RESULT_STATUS_STR   = "STATUS";

typedef enum Status {
    LOCAL, GLOBAL, FAILURE
} STATUS;

const char* resultStr(uint32_t result) {
    if (result == RESULT_SUCCESS) {
        return RESULT_SUCCESS_STR;
    } else if (result == RESULT_ERROR) {
        return RESULT_ERROR_STR;
    } else if (result == RESULT_STATUS) {
        return RESULT_STATUS_STR;
    } else {
        assert(false);
        return NULL;
    }
}

void assertResult(uint32_t expectedResult, struct Ping4jResult* result) {
    printf("\t\tresult=%d, value=%d\n", result->result, result->value);
    asserts++;
    if (result->result == expectedResult) {
        success++;
    } else {
        failure++;
        printf(
            "\t\tFAIL: expected %s (%d), got %s (%d):\n",
            resultStr(expectedResult), expectedResult,
            resultStr(result->result), result->result
        );
    }
}

uint16_t word6(PING4J_IPV6_ADDRESS* address, int no) {
    return ((uint16_t) address->octets[no * 2] << 8) + address->octets[no * 2 + 1];
}

void print(PING4J_IP_ADDRESS* address) {
    if (address->version == PING4J_IPV4_VERSION) {
        printf(
            "%d.%d.%d.%d\n",
             address->u.ipv4.octets[0],
             address->u.ipv4.octets[1],
             address->u.ipv4.octets[2],
             address->u.ipv4.octets[3]
        );
    } else if (address->version == PING4J_IPV6_VERSION) {
        printf(
            "%x:%x:%x:%x:%x:%x:%x:%x\n",
            word6(&address->u.ipv6, 0),
            word6(&address->u.ipv6, 1),
            word6(&address->u.ipv6, 2),
            word6(&address->u.ipv6, 3),
            word6(&address->u.ipv6, 4),
            word6(&address->u.ipv6, 5),
            word6(&address->u.ipv6, 6),
            word6(&address->u.ipv6, 7)
        );
    } else {
        assert(false);
    }
}

void check(
    uint32_t expectedResult,
    PING4J_IP_ADDRESS* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t size
) {
    printf("\tping%d ", address->version);
    print(address);
    printf("\t\ttimeout=%d, ttl=%d, size=%d\n", timeout, ttl, size);

    struct Ping4jResult result;
    ping4jPing(address, timeout, ttl, size, &result);
    assertResult(expectedResult, &result);
}

void checkStatus(STATUS status, PING4J_IP_ADDRESS* address) {
    if (status == FAILURE) {
        check(RESULT_STATUS, address, 100, 100, 100);
    } else {
        check(RESULT_SUCCESS, address, 3000, 100, 100);
        check(RESULT_SUCCESS, address, 3000, 100, 1);

        if (status == GLOBAL){
            check(RESULT_STATUS, address, 1, 100, 100);
            check(RESULT_STATUS, address, 3000, 1, 100);
        }
    }
}

void ping4jDns4v(
    const char* const name,
    PING4J_IP_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
) {
    ping4jDns4(name, (PING4J_IPV4_ADDRESS *) addresses, maxAddresses, result);
}

void ping4jDns6v(
    const char* const name,
    PING4J_IP_ADDRESS* const addresses,
    const size_t maxAddresses,
    PING4J_RESULT* const result
) {
    ping4jDns6(name, (PING4J_IPV6_ADDRESS *) addresses, maxAddresses, result);
}

typedef struct Settings {
    uint8_t version;
    const char* dnsType;
    size_t size;
    void (*query)(const char *, PING4J_IP_ADDRESS*, size_t, PING4J_RESULT* const);
    void (*print)(void *);
} SETTINGS;

static SETTINGS IPV4 = {.version = 4, .size =  4, .query = ping4jDns4v};
static SETTINGS IPV6 = {.version = 6, .size = 16, .query = ping4jDns6v};

#define DNS_BUFFER_SIZE 1000

void dnsV(STATUS status, const char* const name, const SETTINGS* const settings) {
    printf("dns%d %s\n", settings->version, name);

    uint8_t addresses[DNS_BUFFER_SIZE] = {0};
    PING4J_RESULT result;
    settings->query(name, (void*) addresses, DNS_BUFFER_SIZE / settings->size, &result);
    assertResult(RESULT_SUCCESS, &result);

    if (result.result == RESULT_SUCCESS) {
        PING4J_IP_ADDRESS address;
        address.version = settings->version;

        uint8_t* addr = addresses;
        for (uint32_t i = 0; i < result.value; i++) {
            memcpy(&address.u, addr, settings->size);
            checkStatus(status, &address);
            addr += settings->size;
        }
    }
}

void dns4(STATUS status, const char* const name) {
    dnsV(status, name, &IPV4);
}

void dns6(STATUS status, const char* const name) {
    dnsV(status, name, &IPV6);
}

void dns(STATUS status, const char* const name) {
    printf("dns %s\n", name);

    PING4J_IP_ADDRESS addresses[DNS_BUFFER_SIZE / sizeof(PING4J_IP_ADDRESS)];
    PING4J_RESULT result;
    ping4jDns(name, addresses, sizeof(addresses) / sizeof(PING4J_IP_ADDRESS), &result);
    assertResult(RESULT_SUCCESS, &result);

    if (result.result == RESULT_SUCCESS) {
        for (uint32_t i = 0; i < result.value; i++) {
            checkStatus(status, addresses + i);
        }
    }
}

void ipv4(
    STATUS status,
    const uint8_t o0, const uint8_t o1, const uint8_t o2, const uint8_t o3
) {
    PING4J_IP_ADDRESS address;
    address.version = PING4J_IPV4_VERSION,
    address.u.ipv4.octets[0] = o0;
    address.u.ipv4.octets[1] = o1;
    address.u.ipv4.octets[2] = o2;
    address.u.ipv4.octets[3] = o3;

    printf("ipv4 ");
    print(&address);

    checkStatus(status, &address);
}

void ipv6(
    STATUS status,
    const uint8_t o0, const uint8_t o1, const uint8_t o2, const uint8_t o3,
    const uint8_t o4, const uint8_t o5, const uint8_t o6, const uint8_t o7,
    const uint8_t o8, const uint8_t o9, const uint8_t o10, const uint8_t o11,
    const uint8_t o12, const uint8_t o13, const uint8_t o14, const uint8_t o15
) {
    PING4J_IP_ADDRESS address;
    address.version = PING4J_IPV6_VERSION,
    address.u.ipv6.octets[0] = o0;
    address.u.ipv6.octets[1] = o1;
    address.u.ipv6.octets[2] = o2;
    address.u.ipv6.octets[3] = o3;
    address.u.ipv6.octets[4] = o4;
    address.u.ipv6.octets[5] = o5;
    address.u.ipv6.octets[6] = o6;
    address.u.ipv6.octets[7] = o7;
    address.u.ipv6.octets[8] = o8;
    address.u.ipv6.octets[9] = o9;
    address.u.ipv6.octets[10] = o10;
    address.u.ipv6.octets[11] = o11;
    address.u.ipv6.octets[12] = o12;
    address.u.ipv6.octets[13] = o13;
    address.u.ipv6.octets[14] = o14;
    address.u.ipv6.octets[15] = o15;

    printf("ipv6 ");
    print(&address);

    checkStatus(status, &address);
}

int main() {
    ping4jInit();

    ipv4(LOCAL, 127, 0, 0, 1);
    ipv4(FAILURE, 192, 0, 1, 1);
    ipv4(GLOBAL, 8, 8, 8, 8);

    dns4(LOCAL, "127.0.0.1");
    dns4(LOCAL, "127.0.0.127");
    dns4(FAILURE, "192.0.1.1");
    dns4(GLOBAL, "8.8.8.8");
    dns4(LOCAL, "localhost");
    dns4(GLOBAL, "ipv4.google.com");

    ipv6(LOCAL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    ipv6(FAILURE, 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    ipv6(GLOBAL, 0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88);

    dns6(LOCAL, "::1");
    dns6(FAILURE, "2001:0db8::1");
    dns6(GLOBAL, "2001:4860:4860::8888");
    dns6(GLOBAL, "ipv6.google.com");

    dns(LOCAL, "localhost");
    dns(GLOBAL, "google.com");
    dns6(GLOBAL, "google.com");

    if (failure == 0) {
        printf("SUCCESS (%d tests)\n", asserts);
        return 0;
    } else {
        printf("FAILED %d of %d tests\n", failure, asserts);
        return 1;
    }
}
