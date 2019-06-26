package info.kgeorgiy.ping4j;

/**
 * Ping tool interface.
 *
 * @author Georgiy Korneev
 */
public interface Ping {
    /**
     * Pings specified host.
     *
     * @param request host ping request.
     * @return ping result.
     */
    PingResult ping(PingRequest request);
}
