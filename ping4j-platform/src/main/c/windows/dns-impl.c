#include "ping4j-impl.h"

#include <winsock2.h>
#include <windns.h>

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
    const WORD queryType,
    const size_t addressSize,
    const size_t addressOffset
) {
    PDNS_RECORD results;
    DNS_STATUS status = DnsQuery(name, queryType, DNS_QUERY_STANDARD, NULL, &results, NULL);
    if (status != 0) {
        setResult(result, RESULT_ERROR, GetLastError());
        return;
    }

    uint8_t* address = (uint8_t*) addresses;
    uint32_t found = 0;
    for (PDNS_RECORD p = results; p && found < maxAddresses; p = p->pNext) {
        if (p->wType == queryType) {
            memcpy(address, ((uint8_t*) p) + addressOffset, addressSize);
            address += addressIncrement;
            found++;
        }
    }

    DnsRecordListFree(results, DnsFreeRecordList);
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
        DNS_TYPE_A, PING4J_IPV4_ADDRESS_SIZE, offsetof(DNS_RECORD, Data.A.IpAddress)
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
        DNS_TYPE_AAAA, PING4J_IPV6_ADDRESS_SIZE, offsetof(DNS_RECORD, Data.AAAA.Ip6Address)
    );
}
