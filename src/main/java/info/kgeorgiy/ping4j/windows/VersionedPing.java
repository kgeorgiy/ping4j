package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.Ping;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;

import java.net.Inet4Address;
import java.net.Inet6Address;

/**
 * IP version-dependent ping test.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public abstract class VersionedPing implements Ping {
    @Override
    public final PingResult ping(final PingRequest request) {
        if (request.getAddress() instanceof Inet4Address) {
            return ping4(request);
        } else if (request.getAddress() instanceof Inet6Address) {
            return ping6(request);
        } else {
            throw new AssertionError("Unknown address type");
        }
    }

    /** Pings {@link Inet6Address}. */
    protected abstract PingResult ping6(PingRequest request);

    /** Pings {@link Inet4Address}. */
    protected abstract PingResult ping4(PingRequest request);
}
