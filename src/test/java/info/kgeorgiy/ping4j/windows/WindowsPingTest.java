package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;

/**
 * Tests {@link WindowsPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
class WindowsPingTest extends AbstractPingTest {
    WindowsPingTest() {
        super(new WindowsPing(), !System.getProperty("os.name").startsWith("Windows"));
    }
}
