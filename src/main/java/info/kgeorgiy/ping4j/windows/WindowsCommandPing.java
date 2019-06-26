package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.PingException;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;

import java.io.IOException;

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
        try {
            final Process process = new ProcessBuilder().command(
                    "ping", version,
                    "-n", "1",
                    "-l", Integer.toString(request.getPacketSize()),
                    "-i", Integer.toString(request.getTtl()),
                    "-w", Integer.toString(request.getTimeout()),
                    request.getAddress().getHostAddress()
            ).start();
            if (process.waitFor() == 0) {
                return new PingResult(request.getAddress(), 1);
            } else {
                return new PingResult(request.getAddress(), "ping exit code " + process.exitValue());
            }
        } catch (final IOException e) {
            throw new PingException(e, "Cannot create 'ping' process: " + e.getMessage());
        } catch (final InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new PingException(e, "Interrupted");
        }
    }
}
