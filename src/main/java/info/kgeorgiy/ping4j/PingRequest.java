package info.kgeorgiy.ping4j;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;

/**
 * Request to ping a host.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public final class PingRequest {
    /** Host address. */
    private final InetAddress address;

    /** Ping timeout (in milliseconds). */
    private final int timeout;

    /** Maximum time-to-live. */
    private final int ttl;

    /** Ping packet size. */
    private final int packetSize;

    /** Whether to allow packet fragmentation. */
    private final boolean allowFragmentation;

    public PingRequest(final InetAddress address, final int timeout, final int ttl, final int packetSize, final boolean allowFragmentation) {
        this.address = address;
        this.timeout = timeout;
        this.ttl = ttl;
        this.packetSize = packetSize;
        this.allowFragmentation = allowFragmentation;
        if (!(address instanceof Inet4Address) && !(address instanceof Inet6Address)) {
            throw new IllegalArgumentException("Only Inet4Address and Inet6Address are supported");
        }
    }

    public InetAddress getAddress() {
        return this.address;
    }

    public int getTimeout() {
        return this.timeout;
    }

    public int getTtl() {
        return this.ttl;
    }

    public int getPacketSize() {
        return this.packetSize;
    }

    public String toString() {
        return "PingRequest(address=" + this.getAddress() + ", timeout=" + this.getTimeout() + ", ttl=" + this.getTtl() + ", packetSize=" + this.getPacketSize() + ")";
    }

    public boolean getAllowFragmentation() {
        return allowFragmentation;
    }
}
