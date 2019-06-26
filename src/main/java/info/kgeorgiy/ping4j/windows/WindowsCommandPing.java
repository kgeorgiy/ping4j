package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.Ping;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;

import java.net.Inet6Address;

/**
 * Ping based on <code>ping</code> command.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class WindowsCommandPing implements Ping {
    @Override
    public PingResult ping(final PingRequest request) {
        if (request.getAddress() instanceof Inet6Address) {

        }
        return null;
    }
}
