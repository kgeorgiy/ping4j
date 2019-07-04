package info.kgeorgiy.ping4j.macos;

import info.kgeorgiy.ping4j.platform.PlatformPing;

/**
 * Ping4j-platform based ping for Linux.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class MacosPlatformPing extends PlatformPing {
    public MacosPlatformPing() {
        super("macos");
    }
}
