package info.kgeorgiy.ping4j.windows;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;
import com.sun.jna.win32.W32APIOptions;

import java.net.InetAddress;

/**
 * Microsoft Windows Ping4j API.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@SuppressWarnings("WeakerAccess")
public interface Ping4jWindows extends Library {
    /** Library instance. */
    Ping4jWindows INSTANCE = Native.load("ping4j-windows", Ping4jWindows.class, W32APIOptions.DEFAULT_OPTIONS);

    /** Ping succeeded. {@link Result#value} is a round trip time in ms. */
    int RESULT_SUCCESS = 0;

    /** System ping error. {@link Result#value} is error returned by GetLastError(). */
    int RESULT_ERROR = 1;

    /** Invalid ICMP status. {@link Result#value} is ICMP status. */
    int RESULT_STATUS = 2;

    /**
     * IP v4 address.
     *
     * <pre>{@code
     * typedef struct Ping4jIpv4Address {
     *     uint8_t octets[4];
     * } PING4J_IPV4_ADDRESS;
     * }</pre>
     */
    @Structure.FieldOrder({"octets", "guard"})
    class Ipv4Address extends Structure {
        public byte[] octets;
        public byte guard;

        public Ipv4Address(final InetAddress address) {
            octets = address.getAddress();
            assert octets.length == 4;
        }
    }

    /**
     * IP v6 address.
     *
     * <pre>{@code
     * typedef struct Ping4jIpv6Address {
     *     uint8_t octets[16];
     * } PING4J_IPV4_ADDRESS;
     * }</pre>
     */
    @Structure.FieldOrder({"octets", "guard"})
    class Ipv6Address extends Structure {
        public byte[] octets;
        public byte guard;

        public Ipv6Address(final InetAddress address) {
            octets = address.getAddress();
            assert octets.length == 16;
        }
    }

    /**
     * Ping result.
     *
     * <pre>{@code
     * struct Ping4jResult {
     *     uint32_t result;
     *     uint32_t value;
     * } PING4J_RESULT;
     * </pre>
     */
    @Structure.FieldOrder({"result", "value"})
    class Result extends Structure {
        public int result;
        public int value;
    }

    /**
     * Performs ping over IPv4.
     *
     * <pre>{@code
     * void ping4jPing4(
     *     struct Ping4jIpv4Address* address,
     *     uint32_t timeout,
     *     uint8_t ttl,
     *     uint16_t packetSize,
     *     struct Ping4jResult* result
     * );
     * </pre>
     */
    void ping4jPing4(Ipv4Address address, int timeout, byte ttl, short packetSize, Result result);

    /**
     * Performs ping over IPv6.
     *
     * <pre>{@code
     * void ping4jPing6(
     *     struct Ping6jIpv4Address* address,
     *     uint32_t timeout,
     *     uint8_t ttl,
     *     uint16_t packetSize,
     *     struct Ping4jResult* result
     * );
     * </pre>
     */
    void ping4jPing6(Ipv6Address address, int timeout, byte ttl, short packetSize, Result result);
}
