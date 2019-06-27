package info.kgeorgiy.ping4j.generic;

import com.sun.jna.Memory;
import info.kgeorgiy.ping4j.PingRequest;

/**
 * Ping data to send.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public final class PingData {
    private static final int PING_DATA_SIZE = 65536;
    public static final Memory PING_DATA = new Memory(PING_DATA_SIZE);

    static {
        for (int i = 0; i < PingData.PING_DATA_SIZE; i++) {
            PingData.PING_DATA.setByte(i, (byte) i);
        }
    }

    /**
     * Returns request's IPv4 address in network byte order.
     */
    public static int getIp4Address(final PingRequest request) {
        final byte[] address = request.getAddress().getAddress();
        return (address[3] & 0xff) << 24 |
                (address[2] & 0xff) << 16 |
                (address[1] & 0xff) <<  8 |
                (address[0] & 0xff);
    }
}
