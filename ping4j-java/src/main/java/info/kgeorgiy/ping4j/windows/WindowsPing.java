package info.kgeorgiy.ping4j.windows;

import com.sun.jna.Pointer;
import com.sun.jna.platform.win32.Kernel32;
import info.kgeorgiy.ping4j.PingException;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.PingData;
import info.kgeorgiy.ping4j.generic.VersionedPing;

import java.net.Inet6Address;

/**
 * MS Windows-specific ping implementation.
 *
 * @author Georgiy Korneev
 */
public final class WindowsPing extends VersionedPing {
    @Override
    protected PingResult ping6(final PingRequest request) {
        final IpHlpApi.Icmp6Handle icmpHandle = checkHandle(IpHlpApi.INSTANCE.Icmp6CreateFile());
        try {
            final IpHlpApi.Icmp6EchoReply reply = new IpHlpApi.Icmp6EchoReply(request.getPacketSize());
            final int result = IpHlpApi.INSTANCE.Icmp6SendEcho2(
                    icmpHandle,
                    Pointer.NULL,
                    Pointer.NULL,
                    Pointer.NULL,
                    new IpHlpApi.SockaddrIn6(),
                    new IpHlpApi.SockaddrIn6((Inet6Address) request.getAddress()),
                    PingData.PING_DATA,
                    request.getPacketSize(),
                    getRequestOptions(request),
                    reply,
                    reply.size(),
                    request.getTimeout()
            );
            return checkResult(request, result, reply.status, reply.data, reply.roundTripTime);
        } finally {
            closeIcmpHandle(icmpHandle);
        }
    }

    private IpHlpApi.IpOptionInformation getRequestOptions(final PingRequest request) {
        return new IpHlpApi.IpOptionInformation((byte) Math.min(request.getTtl(), 255), request.getAllowFragmentation());
    }

    private PingResult checkResult(
            final PingRequest request,
            final int result,
            final int status,
            final byte[] replyData,
            final int roundTripTime
    ) {
        if (result != 1) {
            final int errorCode = getLastError();
            return new PingResult(request.getAddress(), IpHlpApi.IcmpStatus.valueOf(errorCode));
        } else if (!IpHlpApi.IcmpStatus.IP_SUCCESS.is(status)) {
            return new PingResult(request.getAddress(), IpHlpApi.IcmpStatus.valueOf(status));
        } else if (roundTripTime > request.getTimeout()) {
            return new PingResult(request.getAddress(), IpHlpApi.IcmpStatus.IP_REQ_TIMED_OUT.name());
        } else {
            final int size = Math.min(32, request.getPacketSize());
            for (int i = 0; i < size; i++) {
                if (replyData[i] != i) {
                    return new PingResult(request.getAddress(), "Invalid response");
                }
            }
            return new PingResult(request.getAddress(), roundTripTime);
        }
    }

    private int getLastError() {
        return Kernel32.INSTANCE.GetLastError();
    }

    @Override
    protected PingResult ping4(final PingRequest request) {
        final int ipAddress = PingData.getIp4Address(request);

        final IpHlpApi.Icmp4Handle icmpHandle = checkHandle(IpHlpApi.INSTANCE.IcmpCreateFile());
        try {
            final IpHlpApi.Icmp4EchoReply reply = new IpHlpApi.Icmp4EchoReply(Math.max(8, request.getPacketSize()));
            final int result = IpHlpApi.INSTANCE.IcmpSendEcho2(
                    icmpHandle,
                    Pointer.NULL,
                    Pointer.NULL,
                    Pointer.NULL,
                    ipAddress,
                    PingData.PING_DATA,
                    request.getPacketSize(),
                    getRequestOptions(request),
                    reply,
                    reply.size(),
                    request.getTimeout()
            );
            return checkResult(request, result, reply.status, reply.data, reply.roundTripTime);
        } finally {
            closeIcmpHandle(icmpHandle);
        }
    }

    private <T extends IpHlpApi.IcmpHandle> T checkHandle(final T handle) {
        if (IpHlpApi.INVALID_ICMP_HANDLE_VALUE.equals(handle)) {
            throw lastErrorException("Cannot create ICMP handle");
        }
        return handle;
    }

    private void closeIcmpHandle(final IpHlpApi.IcmpHandle icmpHandle) throws PingException {
        if (!IpHlpApi.INSTANCE.IcmpCloseHandle(icmpHandle)) {
            throw lastErrorException("Cannot close ICMP handle");
        }
    }

    private PingException lastErrorException(final String message) {
        return new PingException(null, message + ": GetLastError() = %d", getLastError());
    }
}
