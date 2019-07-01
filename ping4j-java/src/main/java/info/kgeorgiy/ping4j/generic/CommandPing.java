package info.kgeorgiy.ping4j.generic;

import info.kgeorgiy.ping4j.PingException;
import info.kgeorgiy.ping4j.PingRequest;
import info.kgeorgiy.ping4j.PingResult;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public final class CommandPing {
    public static PingResult ping(final PingRequest request, final String... args) {
        System.out.println(String.join(" ", args));
        try {
            final long start = System.currentTimeMillis();
            final Process process = new ProcessBuilder().command(args).start();
            if (process.waitFor() == 0) {
                final long time = System.currentTimeMillis() - start;
                if (time > request.getTimeout()) {
                    return new PingResult(request.getAddress(), "Timed out");
                } else if (new BufferedReader(new InputStreamReader(process.getInputStream())).lines().anyMatch(line -> line.contains("expired"))) {
                    // Windows workaround
                    return new PingResult(request.getAddress(), "TTL expired in transit");
                } else {
                    return new PingResult(request.getAddress(), (int) time);
                }
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
