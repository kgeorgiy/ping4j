package info.kgeorgiy.ping4j.linux;

import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.CommandPing;
import info.kgeorgiy.ping4j.generic.VersionedPing;

/**
 * Ping based on <code>ping</code> and <code>ping6</code> commands.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class LinuxCommandPing extends VersionedPing {
    @Override
    protected PingResult ping6(final PingRequest request) {
        return ping(request, "ping6");
    }

    @Override
    protected PingResult ping4(final PingRequest request) {
        return ping(request, "ping");
    }

    private PingResult ping(final PingRequest request, final String program) {
        return CommandPing.ping(
                request,
                program,
                "-c", "1",
                "-s", Integer.toString(request.getPacketSize()),
                "--ttl=" + request.getTtl(),
                "-w", Integer.toString(Math.max(1, (request.getTimeout() + 500) / 1000)),
                "-n", "-q",
                request.getAddress().getHostAddress()
        );
    }
}
