package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;

/**
 * Tests {@link WindowsCommandPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
class WindowsCommandPingTest extends AbstractPingTest {
    WindowsCommandPingTest() {
        super(new WindowsCommandPing(), !System.getProperty("os.name").startsWith("Windows"));
    }
}
