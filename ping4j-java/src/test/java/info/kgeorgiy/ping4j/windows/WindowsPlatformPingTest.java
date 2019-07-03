package info.kgeorgiy.ping4j.windows;

import info.kgeorgiy.ping4j.AbstractPingTest;
import info.kgeorgiy.ping4j.platform.PlatformPing;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link PlatformPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.WINDOWS)
class WindowsPlatformPingTest extends AbstractPingTest {
    WindowsPlatformPingTest() {
        super(new WindowsPlatformPing(), false);
    }
}
