package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import info.kgeorgiy.ping4j.Ping;

/**
 * Tests {@link WindowsPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
class WindowsPingTest extends AbstractPingTest {
    private static final Ping INSTANCE = new WindowsPing();

    @Override
    protected Ping getPing() {
        return INSTANCE;
    }
}