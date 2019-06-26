package info.kgeorgiy.ping4j.java;

import info.kgeorgiy.ping4j.Ping;
import info.kgeorgiy.ping4j.PingException;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;

import java.io.IOException;
import java.net.NetworkInterface;

/**
 * Ping implementation based on {@link java.net.InetAddress#isReachable(NetworkInterface, int, int)}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class JavaPing implements Ping {
    @Override
    public PingResult ping(final PingRequest request) {
        try {
            if (request.getAddress().isReachable(null, Math.max(request.getTtl(), 1), Math.max(request.getTimeout(), 1))) {
                return new PingResult(request.getAddress(), 0);
            } else {
                return new PingResult(request.getAddress(), "Unreachable");
            }
        } catch (final IOException e) {
            throw new PingException(e, "JavaPing error: %s", e.getMessage());
        }
    }
}
