package info.kgeorgiy.ping4j.linux;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Test for {@link LinuxPlatformPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.LINUX)
class LinuxPlatformPingTest extends AbstractPingTest {
    LinuxPlatformPingTest() {
        super(new LinuxPlatformPing(), false);
    }
}
