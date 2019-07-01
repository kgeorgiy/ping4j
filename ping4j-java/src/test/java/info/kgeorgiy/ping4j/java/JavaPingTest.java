package info.kgeorgiy.ping4j.java;

import info.kgeorgiy.ping4j.AbstractPingTest;

/**
 * Test for {@link JavaPing}.
 *
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
class JavaPingTest extends AbstractPingTest {
    JavaPingTest() {
        super(new JavaPing(), true);
    }
}