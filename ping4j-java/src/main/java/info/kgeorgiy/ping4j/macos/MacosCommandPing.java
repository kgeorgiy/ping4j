package info.kgeorgiy.ping4j.macos;

import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;
import info.kgeorgiy.ping4j.generic.CommandPing;
import info.kgeorgiy.ping4j.generic.VersionedPing;

/**
 * Ping based on <code>ping</code> and <code>ping6</code> commands.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class MacosCommandPing extends VersionedPing {
    @Override
    protected PingResult ping6(final PingRequest request) {
        return CommandPing.ping(
                request,
                "ping6",
                "-c", "1",
                "-s", Integer.toString(request.getPacketSize()),
                "-h", Integer.toString(request.getTtl()),
                "-n",
                "-q",
                request.getAddress().getHostAddress()
        );
    }

    @Override
    protected PingResult ping4(final PingRequest request) {
        return CommandPing.ping(
                request,
                "ping",
                "-c", "1",
                "-s", Integer.toString(request.getPacketSize()),
                "-m", Integer.toString(request.getTtl()),
                "-t", Integer.toString(Math.max(1, (request.getTimeout() + 500) / 1000)),
                "-n", "-Q",
                request.getAddress().getHostAddress()
        );
    }

}
