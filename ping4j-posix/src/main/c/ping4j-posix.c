#include "ping4j-posix.h"

#define _DEFAULT_SOURCE

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
#define icmphdr icmp
#endif

struct IcmpPacket {
    struct icmphdr header;
    uint8_t body[65000];
};

const size_t MIN_PACKET_SIZE = sizeof(struct icmphdr);

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
static const uint32_t ICMP_INVALID_REPLY = 11011;
static const uint32_t IP_HEADER_SIZE = 20;

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

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (!check(sock, result)) {
        return;
    }

    uint32_t ttl32 = ttl;
    printf("\t\tcreated\n");
    struct timeval timevalue = {.tv_sec = timeout / 1000, .tv_usec = timeout % 1000 * 1000};
    printf("\t\ttimeout = %d, sec = %d, usec = %d\n", timeout, (uint32_t) timevalue.tv_sec, (uint32_t) timevalue.tv_usec);
    if (
        !check(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &REPLY_SIZE, sizeof(REPLY_SIZE)), result) ||
        !check(setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl32, sizeof(ttl32)), result) ||
        !check(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timevalue, sizeof(timevalue)), result)
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
    uint16_t localSeq = seq++;

    static struct IcmpPacket request;
    request.header.type = ICMP_ECHO;
    request.header.code = 0;
    request.header.checksum = 0;
    request.header.un.echo.id = id;
    request.header.un.echo.sequence = localSeq;

    for (unsigned int i = 0; i < packetSize - sizeof(request.header); i++) {
        request.body[i] = (unsigned char) 'a' + i;
    }
    request.header.checksum = checksum(&request, packetSize);

    const struct sockaddr_in toAddress = {.sin_family = AF_INET, .sin_addr = {*(uint32_t *) address->octets}};
    struct timespec start;
    if (!check(clock_gettime(CLOCK_MONOTONIC, &start), result)) {
        close(sock);
        return;
    }

    printf("\t\tstart\n");

    if (!check(sendto(sock, &request, packetSize, 0, (struct sockaddr*) &toAddress, sizeof(toAddress)), result)) {
        close(sock);
        return;
    }
    printf("\t\tsend %x %d\n", toAddress.sin_addr.s_addr, packetSize);

    uint8_t reply[REPLY_SIZE];
    memset(reply, -1, REPLY_SIZE);

    rec: ;
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

    struct timespec end;
    if (!check(clock_gettime(CLOCK_MONOTONIC, &end), result)) {
        close(sock);
        return;
    }

    if (received < IP_HEADER_SIZE) {
        close(sock);
        setResult(result, RESULT_STATUS, ICMP_INVALID_REPLY);
        return;
    }

    printf("\t\treceived: ");
    for (int i = 0; i < received + 10; i++) {
        printf("%02x ", reply[i]);
    }
    printf("\n");

    // IP packet length field
    size_t headerLen = ((*(uint8_t *) reply) & 0xf) * 4;
    printf("\t\theaderLen: %ld\n", headerLen);
    if (received < headerLen + sizeof(struct icmphdr)) {
        close(sock);
        setResult(result, RESULT_STATUS, ICMP_INVALID_REPLY);
        return;
    }

    const struct icmphdr* icmphdr = (struct icmphdr *) (reply + headerLen);
    printf(
        "\t\ttype: %d %d, id: %d %d, seq %d %d, addr %x %x\n",
        ICMP_ECHOREPLY, icmphdr->type,
        id, icmphdr->un.echo.id,
        localSeq, icmphdr->un.echo.sequence,
        toAddress.sin_addr.s_addr, fromAddress.sin_addr.s_addr
    );
    if (icmphdr->type == ICMP_ECHO) {
        goto rec;
    }
    if (
        icmphdr->type != ICMP_ECHOREPLY ||
        icmphdr->un.echo.id != id ||
        icmphdr->un.echo.sequence != localSeq ||
        fromAddress.sin_addr.s_addr != toAddress.sin_addr.s_addr
    ) {
        close(sock);
        setResult(result, RESULT_STATUS, ICMP_INVALID_REPLY);
        return;
    }
    if (!check(close(sock), result)) {
        return;
    }


    uint64_t elapsed = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
    if (elapsed > timeout) {
        setResult(result, RESULT_STATUS, ICMP_TIMED_OUT);
    } else {
        setResult(result, RESULT_SUCCESS, 0);
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
