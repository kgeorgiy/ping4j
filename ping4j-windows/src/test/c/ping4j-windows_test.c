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

void check4(uint32_t expectedResult, PING4J_IPV4_ADDRESS* address, uint32_t timeout, uint8_t ttl, uint16_t size) {
    printf("ping4 ");
    print4(address);
    printf("\ttimeout=%d, ttl=%d, size=%d\n", timeout, ttl, size);

    struct Ping4jResult result;
    ping4jPing4(address, timeout, ttl, size, &result);

    printf("\tresult=%d, value=%d\n", result.result, result.value);
    assert(result.result == expectedResult);
}

void checkSuccess4(PING4J_IPV4_ADDRESS* address, bool global) {
    check4(RESULT_SUCCESS, address, 1000, 100, 100);
    check4(RESULT_SUCCESS, address, 1000, 100, 1);

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

    printf("\tresult=%d, value=%d\n", result.result, result.value);
    assert(result.result == expectedResult);
}

void checkSuccess6(PING4J_IPV6_ADDRESS* address, bool global) {
    check6(RESULT_SUCCESS, address, 10000, 100, 100);
    check6(RESULT_SUCCESS, address, 10000, 100, 1);

    if (global){
        check6(RESULT_STATUS, address, 1, 100, 100);
        check6(RESULT_STATUS, address, 10000, 1, 100);
    }
}

void checkFail6(PING4J_IPV6_ADDRESS* address) {
    check6(RESULT_STATUS, address, 100, 100, 100);
}


int main() {
    PING4J_IPV4_ADDRESS local4 = {{127, 0, 0, 1}};
    PING4J_IPV4_ADDRESS reserved4 = {{192, 0, 1, 1}};
    PING4J_IPV4_ADDRESS localhost4 = dns4("localhost");
    PING4J_IPV4_ADDRESS google4 = dns4("ipv4.google.com");

    PING4J_IPV6_ADDRESS local6 = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS reserved6 = {{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
    PING4J_IPV6_ADDRESS localhost6 = dns6("localhost");
    PING4J_IPV6_ADDRESS google6 = dns6("ipv6.google.com");


    ping4jInit();

    checkSuccess4(&local4, false);
    checkFail4(&reserved4);
    checkSuccess4(&localhost4, false);
    checkSuccess4(&google4, true);

    checkSuccess6(&local6, false);
    checkFail6(&reserved6);
    checkSuccess6(&localhost6, false);
    checkSuccess6(&google6, true);

    printf("SUCCESS\n");
    return 0;
}
