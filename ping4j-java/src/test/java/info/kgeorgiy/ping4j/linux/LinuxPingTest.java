package info.kgeorgiy.ping4j.linux;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link LinuxPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.LINUX)
class LinuxPingTest extends AbstractPingTest {
    LinuxPingTest() {
        super(new LinuxPing(), false);
    }
}
