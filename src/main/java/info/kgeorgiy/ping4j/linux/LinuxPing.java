package info.kgeorgiy.ping4j.linux;

import com.sun.jna.Native;
import com.sun.jna.ptr.IntByReference;
import info.kgeorgiy.ping4j.PingException;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.PingData;
import info.kgeorgiy.ping4j.generic.VersionedPing;

/**
 * Native Linux ping.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class LinuxPing extends VersionedPing {
    @Override
    protected PingResult ping6(final PingRequest request) {
        throw new UnsupportedOperationException();
    }

    @Override
    protected PingResult ping4(final PingRequest request) {
        final int socket = createSocket();
        try {
            setSocketOption("Set receive buffer size", socket, Posix.SOL_SOCKET, Posix.SO_RCVBUF, request.getPacketSize());
            setSocketOption("Set TTL", socket, Posix.IPPROTO_IP, Posix.IP_TTL, request.getTtl());

            final Posix.SockAddrIn sockAddr = new Posix.SockAddrIn((short) 0, PingData.getIp4Address(request));
            System.out.printf("\t\tsockAddr.size() = %d%n", sockAddr.size());
            System.out.printf("\t\tsockAddr.address = %x%n", sockAddr.address);
            check(
                    "Cannot send ICMP request",
                    Posix.INSTANCE.sendto(socket, PingData.PING_DATA, request.getPacketSize(), 0, sockAddr, sockAddr.size())
            );

            return new PingResult(request.getAddress(), 10);
        } finally {
            closeSocket(socket);
        }
    }

    private void closeSocket(final int socket) {
        System.out.format("\t\tClose socket %d%n", socket);
        check("Cannot close socket", Posix.INSTANCE.close(socket));
    }

    private int createSocket() {
        final int socket = Posix.INSTANCE.socket(Posix.AF_INET, Posix.SOCK_RAW, Posix.IPPROTO_ICMP);
        check("Cannot create raw socket", socket);
        System.out.format("\t\tOpen socket %d%n", socket);
        return socket;
    }

    private void setSocketOption(final String message, final int socket, final int level, final int option, final int value) {
        check(message, Posix.INSTANCE.setsockopt(socket, level, option, new IntByReference(value), 4));
    }

    private void check(final String message, final int result) {
        if (result == -1) {
            throw new PingException(null, message + ": errno = %d", Native.getLastError());
        }
    }
}
