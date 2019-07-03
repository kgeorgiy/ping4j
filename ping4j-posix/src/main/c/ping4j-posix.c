#include "ping4j-posix.h"

#include "ping4j-icmp.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#define SOCKET_TYPE SOCK_DGRAM
#else
#define SOCKET_TYPE SOCK_RAW
#endif

struct IcmpPacket {
    PING4J_ICMP_ECHO header;
    uint8_t body[65000];
};

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

bool setResult(struct Ping4jResult* pingResult, uint32_t result, uint32_t value) {
    pingResult->result = result;
    pingResult->value = value;
    return false;
}

static const int REPLY_SIZE = 65530;

struct Request {
    int sock;
    uint8_t reply[65530];
};

bool check(int value, struct Ping4jResult* result) {
    if (value == -1) {
        setResult(result, RESULT_ERROR, errno);
        return false;
    } else {
        return true;
    }
}

static const uint32_t ICMP_TIMED_OUT = 11010;
static const uint32_t IP_HEADER_SIZE = 20;

int64_t currentTimeMillis() {
    struct timespec spec;
    if (clock_gettime(CLOCK_MONOTONIC, &spec) == -1) {
    	return -1;
    }
    return spec.tv_sec * (int64_t) 1000 + spec.tv_nsec / 1000000;
}

uint16_t seq = 0;

void ping4jPing4(
    struct Ping4jIpv4Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    if (packetSize < MIN_PACKET_SIZE) {
        packetSize = MIN_PACKET_SIZE;
    }

    int sock = socket(AF_INET, SOCKET_TYPE, IPPROTO_ICMP);
    if (!check(sock, result)) {
        return;
    }
    printf("\t\tcreated\n");

    uint32_t ttl32 = ttl;
    if (
        !check(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &REPLY_SIZE, sizeof(REPLY_SIZE)), result) ||
        !check(setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl32, sizeof(ttl32)), result)
    ) {
        close(sock);
        return;
    }
    printf("\t\toptions\n");

    uint32_t ttlRes;
    uint32_t ttlSize = sizeof(ttlRes);
    if (!check(getsockopt(sock, IPPROTO_IP, IP_TTL, &ttlRes, &ttlSize), result)) {
        close(sock);
        return;
    }
    printf("\t\tttl = %d\n", ttlRes);

    uint16_t id = getpid();
    uint16_t sequence = seq++;

    static struct IcmpPacket request;
    request.header.type = ICMP_ECHO;
    request.header.code = 0;
    request.header.checksum = 0;
    request.header.identifier = id;
    request.header.sequence = sequence;

    for (unsigned int i = 0; i < packetSize - sizeof(request.header); i++) {
        request.body[i] = (unsigned char) 'a' + i;
    }
    request.header.checksum = checksum(&request, packetSize);

    int64_t current = currentTimeMillis();
    if (!check(current, result)) {
        close(sock);
        return;
    }

    const struct sockaddr_in toAddress = {.sin_family = AF_INET, .sin_addr = {*(uint32_t *) address->octets}};
    if (!check(sendto(sock, &request, packetSize, 0, (struct sockaddr*) &toAddress, sizeof(toAddress)), result)) {
        close(sock);
        return;
    }
    printf("\t\tsend %x %d\n", toAddress.sin_addr.s_addr, packetSize);

    int64_t deadline = current + timeout;

    while (true) {
		int64_t remaining = deadline - current;
    	struct timeval remainingVal = {.tv_sec = remaining / 1000, .tv_usec = remaining % 1000 * 1000};
        if (!check(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &remainingVal, sizeof(remainingVal)), result)) {
        	close(sock);
        	return;
        }

        uint8_t reply[REPLY_SIZE];

        struct sockaddr_in fromAddress;
        socklen_t fromAddressLen = sizeof(fromAddress);

        const ssize_t received = recvfrom(sock, &reply, sizeof(reply), 0, (struct sockaddr*) &fromAddress, &fromAddressLen);
        printf("\t\treceived %ld\n", received);
        if (received == -1 && errno == EAGAIN) {
            close(sock);
            setResult(result, RESULT_STATUS, ICMP_TIMED_OUT);
            return;
        }
        if (!check(received, result)) {
            close(sock);
            return;
        }

        current = currentTimeMillis();
        if (!check(current, result)) {
        	return;
        }

        if (current >= deadline) {
        	close(sock);
            setResult(result, RESULT_STATUS, ICMP_TIMED_OUT);
            return;
        }

        if (received < IP_HEADER_SIZE) {
        	continue;
        }

        printf("\t\treceived: ");
        for (int i = 0; i < received + 10; i++) {
            printf("%02x ", reply[i]);
        }
        printf("\n");

        // IP packet length field
        size_t headerLen = ((*(uint8_t *) reply) & 0xf) * 4;
        printf("\t\theaderLen: %ld\n", headerLen);
        if (received < headerLen + sizeof(PING4J_ICMP_ECHO)) {
        	continue;
        }

        const PING4J_ICMP_ECHO* echo = (PING4J_ICMP_ECHO *) (reply + headerLen);
        printf(
            "\t\ttype: %d %d, id: %d %d, seq %d %d, addr %x %x\n",
            ICMP_ECHOREPLY, echo->type,
            id, echo->identifier,
            sequence, echo->sequence,
            toAddress.sin_addr.s_addr, fromAddress.sin_addr.s_addr
        );
        if (
            echo->type != ICMP_ECHOREPLY ||
            echo->identifier != id ||
            echo->sequence != sequence ||
            fromAddress.sin_addr.s_addr != toAddress.sin_addr.s_addr
        ) {
        	continue;
        }

        if (!check(close(sock), result)) {
            return;
        }
        setResult(result, RESULT_SUCCESS, 0);
        return;
    }
}

void ping4jPing6(
    struct Ping4jIpv6Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    setResult(result, RESULT_ERROR, 0xffff);
}
