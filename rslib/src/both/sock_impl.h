#ifndef RSLIB_SOCK_IMPL_H
#define RSLIB_SOCK_IMPL_H

#include "rslib/both/err.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
/* Windows 95 has no WSAPoll. select is the wait path on that target. */
#if defined(RS_WIN95) || (defined(WINVER) && WINVER < 0x0500) || (defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0500)
#define RS_SOCK_WIN95 1
#ifndef FD_SETSIZE
#define FD_SETSIZE 2048
#endif
#endif
#include <winsock2.h>
#ifndef RS_SOCK_WIN95
#include <ws2tcpip.h>
#endif
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
typedef SOCKET rs_sock;
#define RS_INVALID_SOCK INVALID_SOCKET
#define rs_sock_close closesocket
#define rs_sock_errno WSAGetLastError()
#define RS_SOCK_AGAIN (WSAEWOULDBLOCK)
#define RS_SOCK_INTR WSAEINTR
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifdef RS_SOCK_WIN95
/* Windows 95 has no WSAPoll. */
typedef struct rs_pollfd {
    SOCKET fd;
    short events;
    short revents;
} rs_pollfd;
#ifndef POLLIN
#define POLLIN 0x0300
#endif
#ifndef POLLOUT
#define POLLOUT 0x0010
#endif
#ifndef POLLERR
#define POLLERR 0x0001
#endif
#ifndef POLLHUP
#define POLLHUP 0x0002
#endif
#ifndef POLLNVAL
#define POLLNVAL 0x0004
#endif
int rs_poll(rs_pollfd *fds, unsigned nfds, int timeout_ms);
#else
typedef WSAPOLLFD rs_pollfd;
#define rs_poll WSAPoll
#endif
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int rs_sock;
#define RS_INVALID_SOCK (-1)
#define rs_sock_close close
#define rs_sock_errno errno
#define RS_SOCK_AGAIN EAGAIN
#define RS_SOCK_INTR EINTR
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
typedef struct pollfd rs_pollfd;
#define rs_poll poll
#endif

rs_err rs_sock_init(void);
int rs_set_nonblock(rs_sock fd);
rs_sock rs_sock_listen(const char *host, int port);
rs_sock rs_sock_accept(rs_sock listen_fd, char *addr, size_t addr_cap);
rs_sock rs_sock_connect(const char *host, int port);
int rs_sock_port(rs_sock fd);
ssize_t rs_sock_send(rs_sock fd, const void *data, size_t n);
ssize_t rs_sock_recv(rs_sock fd, void *data, size_t n);
int rs_sock_nodelay(rs_sock fd);

#endif
