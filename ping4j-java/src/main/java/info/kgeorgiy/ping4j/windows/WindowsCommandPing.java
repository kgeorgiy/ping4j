package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.CommandPing;
import info.kgeorgiy.ping4j.generic.VersionedPing;

/**
 * Ping based on <code>ping</code> command.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class WindowsCommandPing extends VersionedPing {
    @Override
    protected PingResult ping6(final PingRequest request) {
        return ping(request, "-6");
    }

    @Override
    protected PingResult ping4(final PingRequest request) {
        return ping(request, "-4");
    }

    private PingResult ping(final PingRequest request, final String version) {
        return CommandPing.ping(
                request,
                "ping", version,
                "-n", "1",
                "-l", Integer.toString(request.getPacketSize()),
                "-i", Integer.toString(request.getTtl()),
                "-w", Integer.toString(request.getTimeout()),
                request.getAddress().getHostAddress()
        );
    }
}
