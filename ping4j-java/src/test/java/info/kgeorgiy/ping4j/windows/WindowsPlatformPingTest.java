package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link WindowsPlatformPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.WINDOWS)
class WindowsPlatformPingTest extends AbstractPingTest {
    WindowsPlatformPingTest() {
        super(new WindowsPlatformPing(), false);
    }
}
