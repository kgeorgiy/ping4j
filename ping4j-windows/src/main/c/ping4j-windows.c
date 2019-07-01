#include "ping4j-windows.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdbool.h>

#ifdef __GNUC__
typedef struct icmpv6_echo_reply {
    IPV6_ADDRESS_EX Address;
    ULONG Status;
    unsigned int RoundTripTime;
} *PICMPV6_ECHO_REPLY;
#endif

static unsigned char DATA[1 << 16];
static SOCKADDR_IN6 fromAddress6;

void ping4jInit() {
    for (unsigned int i = 0; i < sizeof(DATA); i++) {
        DATA[i] = (unsigned char) i;
    }

    ZeroMemory(&fromAddress6, sizeof(fromAddress6));
    fromAddress6.sin6_family = AF_INET6;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(fdwReason);
    UNREFERENCED_PARAMETER(lpvReserved);

    if (fdwReason == DLL_PROCESS_ATTACH) {
        ping4jInit();
    }
    return TRUE;
}

bool setResult(struct Ping4jResult* pingResult, uint32_t result, uint32_t value) {
    pingResult->result = result;
    pingResult->value = value;
    return false;
}

static const DWORD REPLY_SIZE = 65530;

struct Request {
    HANDLE handle;
    uint8_t reply[65530];
    IP_OPTION_INFORMATION options;
};

bool prepare(uint8_t ttl, HANDLE handle, struct Request* request, struct Ping4jResult* result) {
    if (handle == INVALID_HANDLE_VALUE) {
        return setResult(result, RESULT_LAST_ERROR, GetLastError());
    }

    request->handle = handle;
    ZeroMemory(&request->options, sizeof(request->options));
    request->options.Ttl = ttl;

    return true;
}

void finalize(struct Request* request, uint32_t timeout, const DWORD replies, const ULONG status, const ULONG roundTripTime, struct Ping4jResult* result) {
    const DWORD lastError = GetLastError();

    if (!IcmpCloseHandle(request->handle)) {
        setResult(result, RESULT_LAST_ERROR, GetLastError());
        return;
    }

    if (replies == 0) {
        if (lastError != IP_REQ_TIMED_OUT) {
            setResult(result, RESULT_LAST_ERROR, lastError);
        } else {
            setResult(result, RESULT_STATUS, lastError);
        }
        return;
    }

    if (status != IP_SUCCESS) {
        setResult(result, RESULT_STATUS, status);
    } else if (roundTripTime > timeout) {
        setResult(result, RESULT_STATUS, IP_REQ_TIMED_OUT);
    } else {
        setResult(result, RESULT_SUCCESS, roundTripTime);
    }
}

void ping4jPing4(
    struct Ping4jIpv4Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    uint32_t hostAddress;
    memcpy(&hostAddress, &address->octets, 4);

    struct Request request;
    if (!prepare(ttl, IcmpCreateFile(), &request, result)) {
        return;
    }

    const DWORD replies = IcmpSendEcho2(
        request.handle,
        NULL, NULL, NULL,
        hostAddress,
        DATA, packetSize,
        &request.options,
        request.reply, REPLY_SIZE,
        timeout
    );

#ifdef _M_AMD64
    const PICMP_ECHO_REPLY32 reply = (PICMP_ECHO_REPLY32) request.reply;
#else
    const PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY) request.reply;
#endif
    finalize(&request, timeout, replies, reply->Status, reply->RoundTripTime, result);
}

void ping4jPing6(
    struct Ping4jIpv6Address* address,
    uint32_t timeout,
    uint8_t ttl,
    uint16_t packetSize,
    struct Ping4jResult* result
) {
    SOCKADDR_IN6 hostAddress;
    ZeroMemory(&hostAddress, sizeof(hostAddress));
    hostAddress.sin6_family = AF_INET6;
    memcpy(&hostAddress.sin6_addr, &address->octets, 16);

    struct Request request;
    if (!prepare(ttl, Icmp6CreateFile(), &request, result)) {
        return;
    }

    const DWORD replies = Icmp6SendEcho2(
        request.handle,
        NULL, NULL, NULL,
        &fromAddress6, &hostAddress,
        DATA, packetSize,
        &request.options,
        request.reply, REPLY_SIZE,
        timeout
    );

    const PICMPV6_ECHO_REPLY reply = (PICMPV6_ECHO_REPLY) request.reply;
    finalize(&request, timeout, replies, reply->Status, reply->RoundTripTime, result);
}
