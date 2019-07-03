#include "ping4j-posix.h"

#include "ping4j-icmp.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#ifdef __APPLE__
#define SOCKET_TYPE SOCK_DGRAM
#else
#define SOCKET_TYPE SOCK_RAW
#endif

const size_t MIN_PACKET_SIZE = sizeof(PING4J_ICMP_ECHO);

uint16_t checksum(void *buffer, size_t len) {
    uint16_t *buf = buffer;
    uint32_t sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len > 0) {
        sum += *(uint8_t *) buf;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    return ~(sum + (sum >> 16));
}

void ping4jInit() {
}

static const int NO_SOCKET = -1;

typedef struct Context {
    int socket;
    PING4J_RESULT* result;
} CONTEXT;

void setResult(CONTEXT* context, uint32_t result, uint32_t value) {
    context->result->result = result;
    context->result->value = value;

    if (context->socket != NO_SOCKET) {
        close(context->socket);
    }
}

bool check(CONTEXT* context, int value) {
    if (value == -1) {
        setResult(context, RESULT_ERROR, errno);
        return false;
    } else {
        return true;
    }
}

static const int REPLY_SIZE = 65530;
static const int MAX_ADDRESS_SIZE = 28;

static const uint32_t ICMP_TIMED_OUT = 11010;
static const uint32_t IP_HEADER_SIZE = 20;

int64_t currentTimeMillis() {
    struct timespec spec;
    if (clock_gettime(CLOCK_MONOTONIC, &spec) == -1) {
        return -1;
    }
    return spec.tv_sec * (int64_t) 1000 + spec.tv_nsec / 1000000;
}

volatile uint16_t seq = 0;

bool setOption(CONTEXT* context, int level, int name, const void *value, socklen_t length) {
    return check(context, setsockopt(context->socket, level, name, value, length));
}

static void ping(
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    PING4J_RESULT* result,
    const int domain, const int protocol,
    const int ttlOptionLevel, const int ttlOptionName,
    const uint16_t echoRequestType, const uint16_t echoReplyType,
    const size_t addressOffset, const size_t addressLength,
    const struct sockaddr *const toAddress, const socklen_t toAddressLen
) {
    CONTEXT context = {.socket = NO_SOCKET, .result = result};
    const uint16_t size = packetSize < MIN_PACKET_SIZE ? MIN_PACKET_SIZE : packetSize;

    context.socket = socket(domain, SOCKET_TYPE, protocol);
    if (!check(&context, context.socket)) {
        return;
    }

    int ttlInt = ttl;
    if (
        !setOption(&context, SOL_SOCKET, SO_RCVBUF, &REPLY_SIZE, sizeof(REPLY_SIZE)) ||
        !setOption(&context, ttlOptionLevel, ttlOptionName, &ttlInt, sizeof(ttlInt))
    ) {
        return;
    }

    uint16_t identifier = getpid();
    uint16_t sequence = seq++;

    struct IcmpPacket {
        PING4J_ICMP_ECHO header;
        uint8_t body[65000];
    } request;
    request.header.type = echoRequestType;
    request.header.code = 0;
    request.header.checksum = 0;
    request.header.identifier = identifier;
    request.header.sequence = sequence;

    uint16_t bodySize = size - sizeof(request.header);
    for (uint16_t i = 0; i < bodySize; i++) {
        request.body[i] = (unsigned char) 'a' + i;
    }
    request.header.checksum = checksum(&request, size);

    uint64_t current = currentTimeMillis();
    if (!check(&context, current)) {
        return;
    }

    if (!check(&context, sendto(context.socket, &request, size, 0, toAddress, toAddressLen))) {
        return;
    }

    const int64_t deadline = current + timeout;
    while (true) {
        int64_t remaining = deadline - current;

        struct timeval remainingVal = {.tv_sec = remaining / 1000, .tv_usec = remaining % 1000 * 1000};
        if (!setOption(&context, SOL_SOCKET, SO_RCVTIMEO, &remainingVal, sizeof(remainingVal))) {
            return;
        }

        uint8_t reply[REPLY_SIZE];
        uint8_t fromAddress[MAX_ADDRESS_SIZE];
        socklen_t fromAddressLen = toAddressLen;

        const ssize_t received = recvfrom(context.socket, &reply, sizeof(reply), 0, (struct sockaddr*) &fromAddress, &fromAddressLen);

        if (received == -1 && errno == EAGAIN) {
            setResult(&context, RESULT_STATUS, ICMP_TIMED_OUT);
            return;
        }
        if (!check(&context, received)) {
            return;
        }

        current = currentTimeMillis();
        if (!check(&context, current)) {
            return;
        }

        if (current >= deadline) {
            setResult(&context, RESULT_STATUS, ICMP_TIMED_OUT);
            return;
        }

        if (domain == AF_INET && received < IP_HEADER_SIZE) {
            continue;
        }

        // IP packet length
        size_t headerLen = domain == AF_INET ? ((*(uint8_t *) reply) & 0xf) * 4 : 0;
        if (received < headerLen + sizeof(PING4J_ICMP_ECHO)) {
            continue;
        }

        const PING4J_ICMP_ECHO* echo = (PING4J_ICMP_ECHO *) (reply + headerLen);
        if (
            echo->type != echoReplyType ||
            echo->identifier != identifier ||
            echo->sequence != sequence ||
            fromAddressLen != toAddressLen ||
            memcmp(fromAddress + addressOffset, ((uint8_t *) toAddress) + addressOffset, addressLength) != 0
        ) {
            continue;
        }

        if (!check(&context, close(context.socket))) {
            return;
        }
        context.socket = NO_SOCKET;
        setResult(&context, RESULT_SUCCESS, 0);
        return;
    }
}

void ping4jPing4(
    const struct Ping4jIpv4Address* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    struct Ping4jResult* result
) {
    const struct sockaddr_in toAddress = {
        .sin_family = AF_INET,
        .sin_addr = {*(uint32_t *) address->octets}
    };

    ping(
        timeout, ttl, packetSize, result,
        AF_INET, IPPROTO_ICMP,
        IPPROTO_IP, IP_TTL,
        ICMP_ECHO, ICMP_ECHOREPLY,
        offsetof(struct sockaddr_in, sin_addr), 4,
        (const struct sockaddr *) &toAddress, sizeof(toAddress)
    );
}

void ping4jPing6(
    const struct Ping4jIpv6Address* const address,
    const uint32_t timeout,
    const uint8_t ttl,
    const uint16_t packetSize,
    struct Ping4jResult* result
) {
    const struct sockaddr_in6 toAddress = {
        .sin6_family = AF_INET6,
        .sin6_addr = *(struct in6_addr *) address->octets
    };

    ping(
        timeout, ttl, packetSize, result,
        AF_INET6, IPPROTO_ICMPV6,
        IPPROTO_IPV6, IPV6_UNICAST_HOPS,
        ICMP6_ECHO_REQUEST, ICMP6_ECHO_REPLY,
        offsetof(struct sockaddr_in6, sin6_addr), 16,
        (const struct sockaddr *) &toAddress, sizeof(toAddress)
    );
}
