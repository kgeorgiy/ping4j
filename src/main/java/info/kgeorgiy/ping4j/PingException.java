package info.kgeorgiy.ping4j;

/**
 * Thrown when and system error occurred during ping.
 *
 * @author Georgiy Korneev
 */
public final class PingException extends RuntimeException {
    public PingException(final String format, final Object... args) {
        super(String.format(format, args));
    }
}
