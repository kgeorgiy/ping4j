package info.kgeorgiy.ping4j.macos;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Test for {@link MacosPlatformPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.MAC)
class MacosPlatformPingTest extends AbstractPingTest {
    MacosPlatformPingTest() {
        super(new MacosPlatformPing(), false);
    }
}
