#define _POSIX_C_SOURCE 200809L

#include "both/sock_impl.h"

#ifndef _WIN32
#include <errno.h>
#endif

#ifdef _WIN32
static int g_wsa;
#endif

rs_err rs_sock_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (g_wsa) {
        return RS_OK;
    }
    /* Windows 95 has no WSAPoll; Winsock 2.2 may be absent, so try 1.1. */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0 &&
        WSAStartup(MAKEWORD(1, 1), &wsa) != 0) {
        return RS_ERR_IO;
    }
    g_wsa = 1;
#endif
    return RS_OK;
}

int rs_set_nonblock(rs_sock fd) {
#ifdef _WIN32
    u_long one = 1;
    return ioctlsocket(fd, FIONBIO, &one) == 0 ? 0 : -1;
#else
    int fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, fl | O_NONBLOCK);
#endif
}

int rs_sock_nodelay(rs_sock fd) {
    int one = 1;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof(one));
}

static rs_sock sock_stream(int family) {
    rs_sock fd;
#ifdef _WIN32
    fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
#else
    fd = socket(family, SOCK_STREAM, 0);
#endif
    return fd;
}

#ifdef RS_SOCK_WIN95
/* Windows 95 has no getaddrinfo. IPv6 is optional: AAAA failure is not fatal when IPv4 works. */
static int rs_win95_resolve(const char *host, struct sockaddr_in *out, int cap) {
    unsigned long ip;
    struct hostent *he;
    int i;
    int n = 0;

    if (!host || !host[0] || !out || cap <= 0) {
        return 0;
    }
    ip = inet_addr(host);
    if (ip != INADDR_NONE) {
        memset(&out[0], 0, sizeof(out[0]));
        out[0].sin_family = AF_INET;
        out[0].sin_addr.s_addr = ip;
        return 1;
    }
    he = gethostbyname(host);
    if (!he || he->h_addrtype != AF_INET || he->h_length < (int)sizeof(struct in_addr) || !he->h_addr_list) {
        return 0;
    }
    for (i = 0; he->h_addr_list[i] && n < cap; i++) {
        memset(&out[n], 0, sizeof(out[n]));
        out[n].sin_family = AF_INET;
        memcpy(&out[n].sin_addr, he->h_addr_list[i], sizeof(out[n].sin_addr));
        n++;
    }
    return n;
}

/* Windows 95 has no WSAPoll. */
int rs_poll(rs_pollfd *fds, unsigned nfds, int timeout_ms) {
    fd_set rfds, wfds, efds;
    struct timeval tv;
    struct timeval *tvp;
    unsigned i;
    int nvalid = 0;
    int ready;
    int count = 0;

    if (!fds) {
        if (nfds == 0) {
            return 0;
        }
        WSASetLastError(WSAEINVAL);
        return -1;
    }
    if (nfds > FD_SETSIZE) {
        WSASetLastError(WSAENOBUFS);
        return -1;
    }
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    for (i = 0; i < nfds; i++) {
        fds[i].revents = 0;
        if (fds[i].fd == RS_INVALID_SOCK) {
            continue;
        }
        nvalid++;
        if (fds[i].events & POLLIN) {
            FD_SET(fds[i].fd, &rfds);
        }
        if (fds[i].events & POLLOUT) {
            FD_SET(fds[i].fd, &wfds);
        }
        FD_SET(fds[i].fd, &efds);
    }
    if (nvalid == 0) {
        return 0;
    }
    if (timeout_ms < 0) {
        tvp = NULL;
    } else {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        tvp = &tv;
    }
    /* nfds is ignored by Winsock select. */
    ready = select(0, &rfds, &wfds, &efds, tvp);
    if (ready < 0) {
        return -1;
    }
    if (ready == 0) {
        return 0;
    }
    for (i = 0; i < nfds; i++) {
        if (fds[i].fd == RS_INVALID_SOCK) {
            continue;
        }
        if (FD_ISSET(fds[i].fd, &rfds)) {
            fds[i].revents = (short)(fds[i].revents | POLLIN);
        }
        if (FD_ISSET(fds[i].fd, &wfds)) {
            fds[i].revents = (short)(fds[i].revents | POLLOUT);
        }
        if (FD_ISSET(fds[i].fd, &efds)) {
            fds[i].revents = (short)(fds[i].revents | POLLERR);
        }
        if (fds[i].revents) {
            count++;
        }
    }
    return count;
}

rs_sock rs_sock_listen(const char *host, int port) {
    struct sockaddr_in addrs[16];
    int naddr;
    int i;
    int one = 1;
    rs_sock fd = RS_INVALID_SOCK;

    if (rs_sock_init() != RS_OK) {
        return RS_INVALID_SOCK;
    }
    if (!host || !host[0] || strcmp(host, "0.0.0.0") == 0) {
        memset(&addrs[0], 0, sizeof(addrs[0]));
        addrs[0].sin_family = AF_INET;
        addrs[0].sin_addr.s_addr = htonl(INADDR_ANY);
        naddr = 1;
    } else if (strcmp(host, "::") == 0 || strcmp(host, "[::]") == 0) {
        return RS_INVALID_SOCK;
    } else {
        naddr = rs_win95_resolve(host, addrs, 16);
        if (naddr <= 0) {
            return RS_INVALID_SOCK;
        }
    }
    for (i = 0; i < naddr; i++) {
        rs_sock s = sock_stream(AF_INET);
        if (s == RS_INVALID_SOCK) {
            continue;
        }
        addrs[i].sin_port = htons((unsigned short)port);
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(one));
        if (bind(s, (struct sockaddr *)&addrs[i], (int)sizeof(addrs[i])) == 0 && listen(s, 128) == 0) {
            rs_set_nonblock(s);
            fd = s;
            break;
        }
        rs_sock_close(s);
    }
    return fd;
}

int rs_sock_port(rs_sock fd) {
    struct sockaddr_in sa;
    int slen = (int)sizeof(sa);
    if (getsockname(fd, (struct sockaddr *)&sa, &slen) != 0) {
        return -1;
    }
    if (sa.sin_family == AF_INET) {
        return ntohs(sa.sin_port);
    }
    return -1;
}

rs_sock rs_sock_accept(rs_sock listen_fd, char *addr, size_t addr_cap) {
    struct sockaddr_in sa;
    int slen = (int)sizeof(sa);
    rs_sock fd = accept(listen_fd, (struct sockaddr *)&sa, &slen);
    if (fd == RS_INVALID_SOCK) {
        return RS_INVALID_SOCK;
    }
    rs_set_nonblock(fd);
    rs_sock_nodelay(fd);
    if (addr && addr_cap) {
        if (sa.sin_family == AF_INET) {
            const char *s = inet_ntoa(sa.sin_addr);
            if (s) {
                snprintf(addr, addr_cap, "%s", s);
            } else {
                addr[0] = 0;
            }
        } else {
            addr[0] = 0;
        }
    }
    return fd;
}

rs_sock rs_sock_connect(const char *host, int port) {
    struct sockaddr_in addrs[16];
    int naddr;
    int i;
    rs_sock fd = RS_INVALID_SOCK;

    if (!host || rs_sock_init() != RS_OK) {
        return RS_INVALID_SOCK;
    }
    naddr = rs_win95_resolve(host, addrs, 16);
    if (naddr <= 0) {
        return RS_INVALID_SOCK;
    }
    for (i = 0; i < naddr; i++) {
        int err;
        addrs[i].sin_port = htons((unsigned short)port);
        fd = sock_stream(AF_INET);
        if (fd == RS_INVALID_SOCK) {
            continue;
        }
        rs_set_nonblock(fd);
        rs_sock_nodelay(fd);
        if (connect(fd, (struct sockaddr *)&addrs[i], (int)sizeof(addrs[i])) == 0) {
            break;
        }
        err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS) {
            break;
        }
        rs_sock_close(fd);
        fd = RS_INVALID_SOCK;
    }
    return fd;
}
#else
rs_sock rs_sock_listen(const char *host, int port) {
    struct addrinfo hints, *res, *rp;
    char portstr[16];
    rs_sock fd = RS_INVALID_SOCK;
    int one = 1;

    if (rs_sock_init() != RS_OK) {
        return RS_INVALID_SOCK;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (!host || !host[0] || strcmp(host, "0.0.0.0") == 0) {
        host = NULL;
        hints.ai_family = AF_INET;
    } else if (strcmp(host, "::") == 0 || strcmp(host, "[::]") == 0) {
        host = NULL;
        hints.ai_family = AF_INET6;
    }
    if (getaddrinfo(host, portstr, &hints, &res) != 0) {
        return RS_INVALID_SOCK;
    }
    for (rp = res; rp; rp = rp->ai_next) {
        fd = sock_stream(rp->ai_family);
        if (fd == RS_INVALID_SOCK) {
            continue;
        }
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(one));
#ifdef IPV6_V6ONLY
        if (rp->ai_family == AF_INET6) {
            int zero = 0;
            setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&zero, sizeof(zero));
        }
#endif
        if (bind(fd, rp->ai_addr, (int)rp->ai_addrlen) == 0 && listen(fd, 128) == 0) {
            rs_set_nonblock(fd);
            break;
        }
        rs_sock_close(fd);
        fd = RS_INVALID_SOCK;
    }
    freeaddrinfo(res);
    return fd;
}

int rs_sock_port(rs_sock fd) {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    if (getsockname(fd, (struct sockaddr *)&ss, &slen) != 0) {
        return -1;
    }
    if (ss.ss_family == AF_INET) {
        return ntohs(((struct sockaddr_in *)&ss)->sin_port);
    }
    if (ss.ss_family == AF_INET6) {
        return ntohs(((struct sockaddr_in6 *)&ss)->sin6_port);
    }
    return -1;
}

rs_sock rs_sock_accept(rs_sock listen_fd, char *addr, size_t addr_cap) {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    rs_sock fd = accept(listen_fd, (struct sockaddr *)&ss, &slen);
    if (fd == RS_INVALID_SOCK) {
        return RS_INVALID_SOCK;
    }
    rs_set_nonblock(fd);
    rs_sock_nodelay(fd);
    if (addr && addr_cap) {
        if (ss.ss_family == AF_INET) {
            inet_ntop(AF_INET, &((struct sockaddr_in *)&ss)->sin_addr, addr, (socklen_t)addr_cap);
        } else if (ss.ss_family == AF_INET6) {
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&ss)->sin6_addr, addr, (socklen_t)addr_cap);
        } else {
            addr[0] = 0;
        }
    }
    return fd;
}

rs_sock rs_sock_connect(const char *host, int port) {
    struct addrinfo hints, *res, *rp;
    char portstr[16];
    rs_sock fd = RS_INVALID_SOCK;

    if (!host || rs_sock_init() != RS_OK) {
        return RS_INVALID_SOCK;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (getaddrinfo(host, portstr, &hints, &res) != 0) {
        return RS_INVALID_SOCK;
    }
    for (rp = res; rp; rp = rp->ai_next) {
        fd = sock_stream(rp->ai_family);
        if (fd == RS_INVALID_SOCK) {
            continue;
        }
        rs_set_nonblock(fd);
        rs_sock_nodelay(fd);
        if (connect(fd, rp->ai_addr, (int)rp->ai_addrlen) == 0) {
            break;
        }
#ifdef _WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINPROGRESS) {
            break;
        }
#else
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        }
#endif
        rs_sock_close(fd);
        fd = RS_INVALID_SOCK;
    }
    freeaddrinfo(res);
    return fd;
}
#endif

ssize_t rs_sock_send(rs_sock fd, const void *data, size_t n) {
#ifdef _WIN32
    int r = send(fd, (const char *)data, (int)n, 0);
    return r;
#else
    return send(fd, data, n, MSG_NOSIGNAL);
#endif
}

ssize_t rs_sock_recv(rs_sock fd, void *data, size_t n) {
#ifdef _WIN32
    int r = recv(fd, (char *)data, (int)n, 0);
    return r;
#else
    return recv(fd, data, n, 0);
#endif
}
