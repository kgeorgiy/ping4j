package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link WindowsPing4jPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.WINDOWS)
class WindowsPing4jPingTest extends AbstractPingTest {
    WindowsPing4jPingTest() {
        super(new WindowsPing4jPing(), false);
    }
}
