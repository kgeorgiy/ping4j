package info.kgeorgiy.ping4j.windows;

import com.sun.jna.Structure;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.VersionedPing;

import java.net.InetAddress;
import java.util.function.Function;

/**
 * Ping4j-based Micorosoft Windows pinger.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class WindowsPing4jPing extends VersionedPing {
    @Override
    protected PingResult ping4(final PingRequest request) {
        return ping(Ping4jWindows.Ipv4Address::new, Ping4jWindows.INSTANCE::ping4jPing4, request);
    }

    @Override
    protected PingResult ping6(final PingRequest request) {
        return ping(Ping4jWindows.Ipv6Address::new, Ping4jWindows.INSTANCE::ping4jPing6, request);
    }

    private <A extends Structure> PingResult ping(final Function<InetAddress, A> addressF, final Ping4j<A> pingF, final PingRequest request) {
        final A address = addressF.apply(request.getAddress());

        final Ping4jWindows.Result result = new Ping4jWindows.Result();
        pingF.ping(address, request.getTimeout(), (byte) request.getTtl(), (short) request.getPacketSize(), result);

        if (result.result == Ping4jWindows.RESULT_SUCCESS) {
            return new PingResult(request.getAddress(), result.value);
        } else if (result.result == Ping4jWindows.RESULT_ERROR) {
            return new PingResult(request.getAddress(), "GetLastError() = " + result.value);
        } else if (result.result == Ping4jWindows.RESULT_STATUS) {
            return new PingResult(request.getAddress(), "ICMP status = " + result.value);
        } else {
            throw new AssertionError("Unknown Ping4jResult::result = " + result.result);
        }
    }

    private interface Ping4j<A> {
        void ping(A address, int timeout, byte ttl, short packetSize, Ping4jWindows.Result result);
    }
}
