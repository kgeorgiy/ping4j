package info.kgeorgiy.ping4j.platform;

import com.sun.jna.Native;
import com.sun.jna.Structure;
import com.sun.jna.win32.W32APIOptions;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.VersionedPing;

import java.net.InetAddress;
import java.util.function.Function;

/**
 * Ping4j-platform based  pinger.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public abstract class PlatformPing extends VersionedPing {
    /** Library instance. */
    private final Ping4jPlatform platform;

    protected PlatformPing(final String system) {
        String systemArch = System.getProperty("os.arch");
        String arch = systemArch.equals("amd64") ? "x86-64" : systemArch;
        platform = Native.load("ping4j-" + system + "-" + arch, Ping4jPlatform.class, W32APIOptions.DEFAULT_OPTIONS);
    }


    @Override
    protected PingResult ping4(final PingRequest request) {
        return ping(Ping4jPlatform.Ipv4Address::new, platform::ping4jPing4, request);
    }

    @Override
    protected PingResult ping6(final PingRequest request) {
        return ping(Ping4jPlatform.Ipv6Address::new, platform::ping4jPing6, request);
    }

    private <A extends Structure> PingResult ping(final Function<InetAddress, A> addressF, final Ping4j<A> pingF, final PingRequest request) {
        final A address = addressF.apply(request.getAddress());

        final Ping4jPlatform.Result result = new Ping4jPlatform.Result();
        pingF.ping(address, request.getTimeout(), (byte) request.getTtl(), (short) request.getPacketSize(), result);

        if (result.result == Ping4jPlatform.RESULT_SUCCESS) {
            return new PingResult(request.getAddress(), result.value);
        } else if (result.result == Ping4jPlatform.RESULT_ERROR) {
            return new PingResult(request.getAddress(), "error = " + result.value);
        } else if (result.result == Ping4jPlatform.RESULT_STATUS) {
            return new PingResult(request.getAddress(), "ICMP status = " + result.value);
        } else {
            throw new AssertionError("Unknown Ping4jResult::result = " + result.result);
        }
    }

    private interface Ping4j<A> {
        void ping(A address, int timeout, byte ttl, short packetSize, Ping4jPlatform.Result result);
    }
}
