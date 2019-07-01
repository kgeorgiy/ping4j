package info.kgeorgiy.ping4j.linux;

import com.sun.jna.*;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.win32.W32APIOptions;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@SuppressWarnings({"unused", "WeakerAccess"})
public interface Posix extends Library {
    /** Library instance. */
    Posix INSTANCE = Native.load("c", Posix.class, W32APIOptions.DEFAULT_OPTIONS);

    /* IPv4. */
    int AF_INET = 2;

    /* IPv6. */
    int AF_INET6 = 10;

    /* Raw-protocol interface */
    int SOCK_RAW = 3;

    /* Internet Control Message Protocol */
    int IPPROTO_ICMP = 1;

    /* Socket-level option. */
    int SOL_SOCKET = 1;

    /* Receive buffer size socket option. */
    int SO_RCVBUF = 8;

    /* Generic IP protocol */
    int IPPROTO_IP = 0;

    /* Time to live. */
    int IP_TTL = 2;

    /** Man: <pre>{@code int socket(int domain, int type, int protocol);}</pre> */
     int socket(int domain, int type, int protocol);

    /** Man: <pre>{@code int close(int fd);}</pre> */
    int close(int fd);

    /** Man: <pre>{@code int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);}</pre> */
    int getsockopt(int sockfd, int level, int optname, PointerType optval, IntByReference optlen);

    /** Man: <pre>{@code int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen); }</pre> */
    int setsockopt(int sockfd, int level, int optname, PointerType optval, int optlen);

    /**
     * Man:
     * <pre>{@code ssize_t sendto(int socket, const void *message, size_t length, int flags,
     *       const struct sockaddr *dest_addr, socklen_t dest_len)}</pre>
     */
    int sendto(int socket, Pointer message, int length, int flags, SockAddrIn destAddr, int destLen);

    /**
     * Man:
     * <pre>{@code
     *  ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
     *                   struct sockaddr *src_addr, socklen_t *addrlen);
     * }</pre>
     */
    int recvfrom(int sockfd, byte[] buffer, int len, int flags, SockAddrIn addr, IntByReference addrlen);


    /** Man: <pre>{@code pid_t getpid(void);}</pre> */
    int getpid();

    /**
     * Man:
     * <pre>{@code
     * struct sockaddr_in {
     *     sa_family_t    sin_family; // address family: AF_INET
     *     in_port_t      sin_port;   // port in network byte order
     *     struct in_addr sin_addr;   // internet address
     * };
     * }</pre>
     */
    @Structure.FieldOrder({"family", "port", "address", "zero"})
    class SockAddrIn extends Structure {
        public short family = AF_INET;
        public short port;
        public int address;
        public byte[] zero = new byte[8];

        public SockAddrIn() {
        }

        public SockAddrIn(final short port, final int address) {
            this.port = port;
            this.address = address;
        }
    }

    /**
     * Man:
     * <pre>{@code
     *  struct pollfd {
     *      int   fd;         // file descriptor
     *      short events;     // requested events
     *      short revents;    // returned events
     *  };
     * }</pre>
     */
    @Structure.FieldOrder({"descriptor", "events", "returnedEvents"})
    class PollDescriptor extends Structure {
        int descriptor;
        short events;
        short returnedEvents;

        public PollDescriptor(final int descriptor, final short events) {
            this.descriptor = descriptor;
            this.events = events;
        }
    }

    /* There is data to read. */
    short POLLIN = 1;

    /**
     * Man: <pre>{@code int poll(struct pollfd *fds, nfds_t nfds, int timeout);}</pre>
     */
    int poll(PollDescriptor[] fds, long nfds, int timeout);

    /**
     *
     */
    class IcmpPacket extends Structure {
        byte type;
        byte code;
        short checksum;
        byte ih_pptr;

    }
}