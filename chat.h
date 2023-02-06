/*
 * chat.h - Copyright (c) 2020 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __CHAT_H__
#define __CHAT_H__

// ---------------------------------------------------------------------------
// some declarations
// ---------------------------------------------------------------------------

using PollFd     = struct pollfd;
using SockAddrIn = struct sockaddr_in;

// ---------------------------------------------------------------------------
// EndPoint
// ---------------------------------------------------------------------------

class EndPoint
{
public:
    EndPoint()
        : EndPoint(INADDR_ANY, 0)
    {
    }

    EndPoint(const uint16_t port)
        : EndPoint(INADDR_ANY, port)
    {
    }

    EndPoint(const uint32_t addr, const uint16_t port)
        : _endpoint()
    {
        _endpoint.sin_family      = AF_INET;
        _endpoint.sin_addr.s_addr = htonl(addr);
        _endpoint.sin_port        = htons(port);
    }

    auto data() -> sockaddr*
    {
        return reinterpret_cast<sockaddr*>(&_endpoint);
    }

    auto data() const -> const sockaddr*
    {
        return reinterpret_cast<const sockaddr*>(&_endpoint);
    }

    auto size() const -> socklen_t
    {
        return sizeof(_endpoint);
    }

private:
    SockAddrIn _endpoint;
};

// ---------------------------------------------------------------------------
// Socket
// ---------------------------------------------------------------------------

class Socket
{
public:
    Socket();

    Socket(const int fd);

    virtual ~Socket();

    int fd() const
    {
        return _fd;
    }

    void reset(const int fd)
    {
        _fd = (close(), fd);
    }

    void create();

    void close();

    void bind(const uint32_t addr, const uint16_t port);

    void listen(const int backlog);

    int  accept();

    void send(const std::string&);

    void recv(std::string&);

    bool get_acceptconn() const;

    bool get_keepalive() const;

    void set_keepalive(const bool value) const;

    bool get_reuseaddr() const;

    void set_reuseaddr(const bool value) const;

    int  get_sndbuf() const;

    void set_sndbuf(const int value) const;

    int  get_rcvbuf() const;

    void set_rcvbuf(const int value) const;

protected:
    int _fd;
};

// ---------------------------------------------------------------------------
// Chat
// ---------------------------------------------------------------------------

class Chat
{
public:
    Chat();

    virtual ~Chat() = default;

    void run(const uint32_t addr, const uint16_t port);

private:
    static constexpr int BACKLOG      = 5;
    static constexpr int STDIN_INDEX  = 0;
    static constexpr int STDOUT_INDEX = 1;
    static constexpr int STDERR_INDEX = 2;
    static constexpr int SERVER_INDEX = 3;
    static constexpr int CLIENT_INDEX = 4;

    void quit();

    void on_error(PollFd& pollable);

    void on_stdin(PollFd& pollable);

    void on_stdout(PollFd& pollable);

    void on_stderr(PollFd& pollable);

    void on_server(PollFd& pollable);

    void on_client(PollFd& pollable);

private:
    std::array<PollFd, 5> _pollables;
    Socket                _server;
    Socket                _client;
    bool                  _quit;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __CHAT_H__ */
