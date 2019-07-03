package info.kgeorgiy.ping4j.linux;

import info.kgeorgiy.ping4j.platform.PlatformPing;

/**
 * Ping4j-platform based ping for Linux.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class LinuxPlatformPing extends PlatformPing {
    public LinuxPlatformPing() {
        super("linux");
    }
}
