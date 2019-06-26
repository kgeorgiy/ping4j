package info.kgeorgiy.ping4j.windows;

import com.sun.jna.*;
import com.sun.jna.platform.win32.WinNT;
import com.sun.jna.win32.W32APIOptions;

import java.net.Inet6Address;
import java.net.UnknownHostException;
import java.util.Arrays;

/**
 * Windows IP Helper API (<code>iphlpapi.dll</code>).
 *
 * @author Georgiy Korneev
 */
@SuppressWarnings({"unused", "WeakerAccess"})
interface IpHlpApi extends Library {
    /** Library instance. */
    IpHlpApi INSTANCE = Native.load("IPHlpAPI", IpHlpApi.class, W32APIOptions.DEFAULT_OPTIONS);

    /** Invalid return value for {@link #IcmpCreateFile()} and {@link #Icmp6CreateFile()}. */
    IcmpHandle INVALID_ICMP_HANDLE_VALUE = new IcmpHandle() {{
        setPointer(Pointer.createConstant(-1));
    }};

    /** Generic ICMP handle. */
    class IcmpHandle extends WinNT.HANDLE {}

    /** IPv4 ICMP handle. */
    class Icmp4Handle extends IcmpHandle {}

    /** IPv6 ICMP handle. */
    class Icmp6Handle extends IcmpHandle {}

    /**
     * From MSDN:
     * <pre>{@code
     *    typedef struct icmp_echo_reply32 {
     *        IPAddr                       Address;
     *        ULONG                        Status;
     *        ULONG                        RoundTripTime;
     *        USHORT                       DataSize;
     *        USHORT                       Reserved;
     *        VOID POINTER_32              Data;
     *        struct ip_option_information Options;
     *    } ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;
     * }</pre>
     */
    @Structure.FieldOrder({"ipAddr", "status", "roundTripTime", "dataSize", "reserved", "data32", "options", "data"})
    class Icmp4EchoReply extends Structure {
        public int ipAddr;
        public int status;
        public int roundTripTime;
        public short dataSize;
        public short reserved;
        public int data32;
        public IpOptionInformation options;
        public byte[] data;

        public Icmp4EchoReply(final int dataSize) {
            this.data = new byte[Math.max(8, dataSize)];
        }
    }

    /**
     * From MSDN:
     * <pre>{@code
     *    typedef struct icmpv6_echo_reply_lh {
     *        IPV6_ADDRESS_EX Address;
     *        ULONG           Status;
     *        unsigned int    RoundTripTime;
     *    } ICMPV6_ECHO_REPLY, *PICMPV6_ECHO_REPLY;
     * }</pre>
     */
    @Structure.FieldOrder({"address", "status", "roundTripTime", "data"})
    class Icmp6EchoReply extends Structure {
        public SockaddrIn6 address = new SockaddrIn6();
        public int status;
        public int roundTripTime;
        public byte[] data;

        public Icmp6EchoReply(final int dataSize) {
            data = new byte[Math.max(8, dataSize)];
        }

        @Override
        public String toString() {
            return "Icmp6EchoReply{" +
                    "address=" + address +
                    ", status=" + IcmpStatus.valueOf(status) +
                    ", roundTripTime=" + roundTripTime +
                    ", data=" + Arrays.toString(data) +
                    '}';
        }
    }

    /**
     * From MSDN:
     * <pre>{@code
     *    typedef struct _IPV6_ADDRESS_EX {
     *        USHORT sin6_port;
     *        ULONG  sin6_flowinfo;
     *        USHORT sin6_addr[8];
     *        ULONG  sin6_scope_id;
     *    } IPV6_ADDRESS_EX, *PIPV6_ADDRESS_EX;
     * }</pre>
     */
    @Structure.FieldOrder({"sin6_port", "sin6_flowinfo", "sin6_addr", "sin6_scope_id"})
    class SockaddrIn6 extends Structure {
        public short sin6_port;
        public int sin6_flowinfo;
        public byte[] sin6_addr = new byte[16];
        public int sin6_scope_id;

        public SockaddrIn6() {
        }

        public SockaddrIn6(final Inet6Address address) {
            sin6_addr = address.getAddress();
        }

        @Override
        public String toString() {
            return "SockaddrIn6{" +
                    "sin6_port=" + sin6_port +
                    ", sin6_flowinfo=" + sin6_flowinfo +
                    ", sin6_scope_id=" + sin6_scope_id +
                    ", sin6_addr=" + bytesToInet6Address(sin6_addr) +
                    '}';
        }

        public static String bytesToInet6Address(final byte[] address) {
            try {
                return Inet6Address.getByAddress(address).toString();
            } catch (UnknownHostException e) {
                throw new AssertionError(e);
            }
        }
    }

    /**
     * From MSDN:
     * <pre>{@code
     *    typedef struct ip_option_information {
     *        UCHAR  Ttl;
     *        UCHAR  Tos;
     *        UCHAR  Flags;
     *        UCHAR  OptionsSize;
     *        PUCHAR OptionsData;
     *    } IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;
     * }</pre>
     */
    @Structure.FieldOrder({"timeToLive", "typeOfService", "flags", "optionsSize", "optionsData"})
    class IpOptionInformation extends Structure {
        public byte timeToLive;
        public byte typeOfService;
        public byte flags;
        public byte optionsSize;
        public Pointer optionsData;

        public IpOptionInformation() {
            this((byte) 0, true);
        }

        public IpOptionInformation(final byte timeToLive, final boolean allowFragmentation) {
            this.timeToLive = timeToLive;
            if (!allowFragmentation) {
                flags = 0x02; // Do not defragment
            }
            optionsData = Pointer.NULL;
        }
    }

    /**
     * From MSDN:
     * <pre>{@code HANDLE IcmpCreateFile(void);}</pre>
     */
    Icmp4Handle IcmpCreateFile();

    /**
     * From MSDN:
     * <pre>{@code HANDLE IcmpCreateFile(void);}</pre>
     */
    Icmp6Handle Icmp6CreateFile();

    /**
     * From MSDN:
     * <pre>{@code
     *    BOOL IcmpCloseHandle(
     *            __in  HANDLE IcmpHandle
     *    );
     * }</pre>
     */
    boolean IcmpCloseHandle(IcmpHandle icmpHandle);

    /**
     * From MSDN:
     * <pre>{@code
     *    DWORD IcmpSendEcho(
     *            __in     HANDLE IcmpHandle,
     *            __in     IPAddr DestinationAddress,
     *            __in     LPVOID RequestData,
     *            __in     WORD RequestSize,
     *            __in     PIP_OPTION_INFORMATION RequestOptions,
     *            __inout  LPVOID ReplyBuffer,
     *            __in     DWORD ReplySize,
     *            __in     DWORD Timeout
     *    );
     * }</pre>
     */
    int IcmpSendEcho(
            Icmp4Handle icmpHandle,
            int destinationAddress,
            byte[] requestData,
            int requestSize,
            Pointer requestOptions,
            Icmp4EchoReply replyBuffer,
            int replySize,
            int timeout
    );

    /**
     * From MSDN:
     * <pre>{@code
     *    DWORD WINAPI IcmpSendEcho2(
     *            __in      HANDLE IcmpHandle,
     *            __in_opt  HANDLE Event,
     *            __in_opt  PIO_APC_ROUTINE ApcRoutine,
     *            __in_opt  PVOID ApcContext,
     *            __in      IPAddr DestinationAddress,
     *            __in      LPVOID RequestData,
     *            __in      WORD RequestSize,
     *            __in_opt  PIP_OPTION_INFORMATION RequestOptions,
     *            __out     LPVOID ReplyBuffer,
     *            __in      DWORD ReplySize,
     *            __in      DWORD Timeout
     *    );
     * }</pre>
     */
    int IcmpSendEcho2(
            Icmp4Handle IcmpHandle,
            Pointer Event,
            Pointer ApcRoutine,
            Pointer ApcContext,
            int destinationAddress,
            Pointer requestData,
            int requestSize,
            IpOptionInformation requestOptions,
            Icmp4EchoReply replyBuffer,
            int replySize,
            int timeout
    );

    /**
     * From MSDN:
     * <pre>{@code
     *    DWORD Icmp6SendEcho2(
     *            __in      HANDLE IcmpHandle,
     *            __in_opt  HANDLE Event,
     *            __in_opt  PIO_APC_ROUTINE ApcRoutine,
     *            __in_opt  PVOID ApcContext,
     *            __in      struct sockaddr_in6 *SourceAddress,
     *            __in      struct sockaddr_in6 *DestinationAddress,
     *            __in      LPVOID RequestData,
     *            __in      WORD RequestSize,
     *            __in_opt  PIP_OPTION_INFORMATION RequestOptions,
     *            __out     LPVOID ReplyBuffer,
     *            __in      DWORD ReplySize,
     *            __in      DWORD Timeout
     *    );
     */
    int Icmp6SendEcho2(
            Icmp6Handle IcmpHandle,
            Pointer Event,
            Pointer ApcRoutine,
            Pointer ApcContext,
            SockaddrIn6 SourceAddress,
            SockaddrIn6 DestinationAddress,
            Pointer requestData,
            int requestSize,
            IpOptionInformation requestOptions,
            Icmp6EchoReply replyBuffer,
            int replySize,
            int timeout
    );

    enum IcmpStatus {
        IP_SUCCESS(0),
        IP_BUF_TOO_SMALL(11001),
        IP_DEST_NET_UNREACHABLE(11002),
        IP_DEST_HOST_UNREACHABLE(11003),
        IP_DEST_PROT_UNREACHABLE(11004),
        IP_DEST_PORT_UNREACHABLE(11005),
        IP_NO_RESOURCES(11006),
        IP_BAD_OPTION(11007),
        IP_HW_ERROR(11008),
        IP_PACKET_TOO_BIG(11009),
        IP_REQ_TIMED_OUT(11010),
        IP_BAD_REQ(11011),
        IP_BAD_ROUTE(11012),
        IP_TTL_EXPIRED_TRANSIT(11013),
        IP_TTL_EXPIRED_REASSEM(11014),
        IP_PARAM_PROBLEM(11015),
        IP_SOURCE_QUENCH(11016),
        IP_OPTION_TOO_BIG(11017),
        IP_BAD_DESTINATION(11018),
        IP_GENERAL_FAILURE(11050);

        public final int code;
        IcmpStatus(final int code) {
            this.code = code;
        }

        public static String valueOf(final int code) {
            for (final IcmpStatus status : values()) {
                if (status.is(code)) {
                    return status.name();
                }
            }
            return "GetLastError() = " + code;
        }

        public boolean is(final int code) {
            return this.code == code;
        }
    }
}
