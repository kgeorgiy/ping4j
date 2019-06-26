package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link WindowsCommandPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.WINDOWS)
class WindowsCommandPingTest extends AbstractPingTest {
    WindowsCommandPingTest() {
        super(new WindowsCommandPing(), !System.getProperty("os.name").startsWith("Windows"));
    }
}
