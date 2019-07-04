package info.kgeorgiy.ping4j.macos;

import info.kgeorgiy.ping4j.AbstractPingTest;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

/**
 * Tests {@link MacosCommandPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@EnabledOnOs(OS.MAC)
class MacosCommandPingTest extends AbstractPingTest {
    MacosCommandPingTest() {
        super(new MacosCommandPing(), false);
    }
}