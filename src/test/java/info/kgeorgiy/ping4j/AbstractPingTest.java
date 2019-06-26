package info.kgeorgiy.ping4j;

import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Abstract base class for ping implementations.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public abstract class AbstractPingTest {
    private static final int TEST_TIMEOUT = 1000;
    private static final int TEST_RTT = 100;
    private static final int PACKET_SIZE = 32;
    private static final int JUMBO_PACKET_SIZE = 32000;

    private final Ping ping;
    private final boolean mayFail;

    public AbstractPingTest(final Ping ping, final boolean mayFail) {
        this.ping = ping;
        this.mayFail = mayFail;
    }

    @Test void testGoogle() throws UnknownHostException {
        checkSuccess(InetAddress.getByName("google.com"), false);
    }

    @Test void testGoogleV4() throws UnknownHostException {
        checkSuccess(InetAddress.getByName("ipv4.google.com"), false);
    }

    @Test void testGoogleV6() throws UnknownHostException {
        checkSuccess(InetAddress.getByName("ipv6.google.com"), false);
    }

    @Test void testLoopbackV4() throws IOException {
        checkSuccess(InetAddress.getByName("127.0.0.1"), true);
    }

    @Test void testLoopbackV6() throws IOException {
        checkSuccess(InetAddress.getByName("::1"), true);
    }

    @Test void testLocalhost() throws IOException {
        for (final InetAddress address : InetAddress.getAllByName("localhost")) {
            checkSuccess(address, true);
        }
    }

    @Test void testReservedV4() throws IOException {
        checkFail(InetAddress.getByName("192.0.2.1"));
    }

    @Test void testReservedV6() throws IOException {
        checkFail(InetAddress.getByName("2001:db8::1"));
    }

    private void checkSuccess(final InetAddress address, final boolean local) {
        checkSuccess(address, PACKET_SIZE, local);
        checkFail(address, 0, TEST_RTT, PACKET_SIZE);

        if (local) {
            checkSuccess(address, JUMBO_PACKET_SIZE, true);
        } else {
            checkFail(address, TEST_TIMEOUT, 0, PACKET_SIZE);
            checkFail(address, TEST_TIMEOUT, TEST_RTT, JUMBO_PACKET_SIZE);
        }
    }

    private void checkSuccess(final InetAddress address, final int packetSize, final boolean local) {
        final PingResult result = ping(address, TEST_TIMEOUT, TEST_RTT, packetSize);
        assertTrue(mayFail || result.isSuccess());
        assertTrue(mayFail || local || result.getRoundTripTime() > 0);
    }

    private void checkFail(final InetAddress address, final int timeout, final int rtt, final int packetSize) {
        assertFalse(ping(address, timeout, rtt, packetSize).isSuccess() && !mayFail);
    }

    private void checkFail(final InetAddress address) {
        checkFail(address, TEST_TIMEOUT, TEST_RTT, PACKET_SIZE);
    }

    private PingResult ping(final InetAddress address, final int timeout, final int rtt, final int packetSize) {
        final PingRequest request = new PingRequest(address, timeout, rtt, packetSize, true);
        System.out.println("Pinging " + request);
        final PingResult result = ping.ping(request);
        System.out.println("\t" + result);
        return result;
    }
}
