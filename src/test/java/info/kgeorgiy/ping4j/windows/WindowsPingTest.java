package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link WindowsPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.WINDOWS)
class WindowsPingTest extends AbstractPingTest {
    WindowsPingTest() {
        super(new WindowsPing(), !System.getProperty("os.name").startsWith("Windows"));
    }
}
