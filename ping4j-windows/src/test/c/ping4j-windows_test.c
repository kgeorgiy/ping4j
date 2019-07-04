#include "ping4j.h"
#include <stdio.h>
#include <assert.h>
#include <winsock2.h>
#include <windns.h>
#include <stdbool.h>

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

PING4J_IPV4_ADDRESS dns4(PCSTR name) {
    printf("DNS type A query for %s\n", name);
    PDNS_RECORD results;
    DNS_STATUS status = DnsQuery(name, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &results, NULL);
    assert(status == 0);

    PING4J_IPV4_ADDRESS address = {{0}};
    bool found = false;
    while (results) {
        if (results->wType == DNS_TYPE_A) {
            address = *(PING4J_IPV4_ADDRESS *) &(results->Data.A.IpAddress);
            found = true;
            printf("\t%s: ", results->pName);
            print4(&address);
        }
        results = results->pNext;
    }
    DnsRecordListFree(results, DnsFreeRecordList);

    assert(found);
    return address;
}

PING4J_IPV6_ADDRESS dns6(PCSTR name) {
    printf("DNS type AAAA query for %s\n", name);
    PDNS_RECORD results;
    DNS_STATUS status = DnsQuery(name, DNS_TYPE_AAAA, DNS_QUERY_STANDARD, NULL, &results, NULL);
    assert(status == 0);

    PING4J_IPV6_ADDRESS address = {{0}};
    bool found = false;
    while (results) {
        if (results->wType == DNS_TYPE_AAAA) {
            address = *(PING4J_IPV6_ADDRESS *) &(results->Data.AAAA.Ip6Address);
            found = true;
            printf("\t%s: ", results->pName);
            print6(&address);
        }
        results = results->pNext;
    }
    DnsRecordListFree(results, DnsFreeRecordList);

    assert(found);
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
    } else {
        assert(false);
        return NULL;
    }
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
        check4(RESULT_STATUS, address, 1000, 1, 100);
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


int main() {
    PING4J_IPV4_ADDRESS local4 = {{127, 0, 0, 1}};
    PING4J_IPV4_ADDRESS reserved4 = {{192, 0, 1, 1}};
    PING4J_IPV4_ADDRESS localhost4 = dns4("localhost");
    PING4J_IPV4_ADDRESS googleDns4 = {{8, 8, 8, 8}};
    PING4J_IPV4_ADDRESS google4 = dns4("ipv4.google.com");

    PING4J_IPV6_ADDRESS local6 = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS reserved6 = {{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS googleDns6 = {{0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88}};
    PING4J_IPV6_ADDRESS google6 = dns6("ipv6.google.com");

    ping4jInit();

    checkSuccess4(&local4, false);
    checkFail4(&reserved4);
    checkSuccess4(&localhost4, false);
    checkSuccess4(&googleDns4, true);
    checkSuccess4(&google4, true);

    checkSuccess6(&local6, false);
    checkFail6(&reserved6);
    checkSuccess6(&googleDns6, true);
    checkSuccess6(&google6, true);

    if (failure == 0) {
        printf("SUCCESS (%d tests)\n", asserts);
        return 0;
    } else {
        printf("FAILED %d of %d tests\n", failure, asserts);
        return 1;
    }
}
