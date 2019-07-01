package info.kgeorgiy.ping4j;

import java.net.InetAddress;
import java.util.Objects;

/**
 * Result of ping attempt.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public final class PingResult {
    /** Pinged address. */
    private final InetAddress address;

    /** Error message. Is {@code null} iff ping was successful. */
    private final String errorMessage;

    /** Ping round trip time. */
    private final int roundTripTime;

    /**
     * Creates successful ping result.
     *
     * @param address ping address.
     * @param roundTripTime ping round trip time.
     */
    public PingResult(InetAddress address, final int roundTripTime) {
        this.address = address;
        this.errorMessage = null;
        this.roundTripTime = roundTripTime;
    }

    /**
     * Creates public ping result.
     * @param address      ping address.
     * @param errorMessage ping error message.
     */
    public PingResult(final InetAddress address, final String errorMessage) {
        this.address = address;
        this.errorMessage = Objects.requireNonNull(errorMessage);
        roundTripTime = 0;
    }

    /**
     * Returns whether ping was successful.
     */
    public boolean isSuccess() {
        return errorMessage == null;
    }

    public InetAddress getAddress() {
        return this.address;
    }

    public String getErrorMessage() {
        return this.errorMessage;
    }

    public int getRoundTripTime() {
        return this.roundTripTime;
    }

    public String toString() {
        return "PingResult(address=" + this.getAddress() + ", errorMessage=" + this.getErrorMessage() + ", roundTripTime=" + this.getRoundTripTime() + ")";
    }
}
