/*
 * chat.cc - Copyright (c) 2020 - Olivier Poncet
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <csignal>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "chat.h"

// ---------------------------------------------------------------------------
// Socket
// ---------------------------------------------------------------------------

Socket::Socket()
    : Socket(-1)
{
}

Socket::Socket(const int fd)
    : _fd(fd)
{
}

Socket::~Socket()
{
    try {
        close();
    }
    catch(const std::exception& e) {
        static_cast<void>(e);
    }
}

void Socket::create()
{
    if(_fd < 0) {
        const int rc = ::socket(AF_INET, SOCK_STREAM, 0);
        if(rc >= 0) {
            _fd = rc;
        }
        else {
            throw std::runtime_error("socket() has failed");
        }
    }
}

void Socket::close()
{
    if(_fd >= 0) {
        const int rc = ::close(_fd);
        if(rc == 0) {
            _fd = -1;
        }
        else {
            throw std::runtime_error("close has failed");
        }
    }
}

void Socket::bind(const uint32_t addr, const uint16_t port)
{
    const EndPoint endpoint(addr, port);

    const int rc = ::bind(_fd, endpoint.data(), endpoint.size());
    if(rc < 0) {
        throw std::runtime_error("bind() has failed");
    }
}

void Socket::listen(const int backlog)
{
    const int rc = ::listen(_fd, backlog);
    if(rc < 0) {
        throw std::runtime_error("listen() has failed");
    }
}

int Socket::accept()
{
    SockAddrIn addr = {};
    socklen_t  size = sizeof(addr);
    const int rc = ::accept(_fd, reinterpret_cast<sockaddr*>(&addr), &size);
    if(rc < 0) {
        throw std::runtime_error("accept() has failed");
    }
    return rc;
}

void Socket::send(const std::string& string)
{
    const ssize_t rc = ::send(_fd, string.data(), string.size(), 0);
    if(rc < 0) {
        throw std::runtime_error("send() has failed");
    }
}

void Socket::recv(std::string& string)
{
    char data[1024];

    const ssize_t rc = ::recv(_fd, data, sizeof(data), 0);
    if(rc < 0) {
        throw std::runtime_error("recv() has failed");
    }
    else if(rc == 0) {
        close();
    }
    else {
        std::string(data, rc).swap(string);
    }
}

bool Socket::get_acceptconn() const
{
    int       option_val = 0;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::getsockopt(_fd, SOL_SOCKET, SO_ACCEPTCONN, &option_val, &option_len);
    if(rc < 0) {
        throw std::runtime_error("getsockopt() has failed");
    }
    return option_val;
}

bool Socket::get_keepalive() const
{
    int       option_val = 0;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::getsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &option_val, &option_len);
    if(rc < 0) {
        throw std::runtime_error("getsockopt() has failed");
    }
    return option_val;
}

void Socket::set_keepalive(const bool value) const
{
    int       option_val = value;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &option_val, option_len);
    if(rc < 0) {
        throw std::runtime_error("setsockopt() has failed");
    }
}

bool Socket::get_reuseaddr() const
{
    int       option_val = 0;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::getsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &option_val, &option_len);
    if(rc < 0) {
        throw std::runtime_error("getsockopt() has failed");
    }
    return option_val;
}

void Socket::set_reuseaddr(const bool value) const
{
    int       option_val = value;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &option_val, option_len);
    if(rc < 0) {
        throw std::runtime_error("setsockopt() has failed");
    }
}

int Socket::get_sndbuf() const
{
    int       option_val = 0;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &option_val, &option_len);
    if(rc < 0) {
        throw std::runtime_error("getsockopt() has failed");
    }
    return option_val;
}

void Socket::set_sndbuf(const int value) const
{
    int       option_val = value;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &option_val, option_len);
    if(rc < 0) {
        throw std::runtime_error("setsockopt() has failed");
    }
}

int Socket::get_rcvbuf() const
{
    int       option_val = 0;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &option_val, &option_len);
    if(rc < 0) {
        throw std::runtime_error("getsockopt() has failed");
    }
    return option_val;
}

void Socket::set_rcvbuf(const int value) const
{
    int       option_val = value;
    socklen_t option_len = sizeof(option_val);
    const int rc = ::setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &option_val, option_len);
    if(rc < 0) {
        throw std::runtime_error("setsockopt() has failed");
    }
}

// ---------------------------------------------------------------------------
// Chat
// ---------------------------------------------------------------------------

Chat::Chat()
    : _pollables()
    , _server()
    , _client()
    , _quit(false)
{
    int index = 0;
    for(auto& pollable : _pollables) {
        switch(index) {
            case STDIN_INDEX:
                pollable.fd      = STDIN_FILENO;
                pollable.events  = POLLIN;
                pollable.revents = 0;
                break;
            case STDOUT_INDEX:
                pollable.fd      = STDOUT_FILENO;
                pollable.events  = 0;
                pollable.revents = 0;
                break;
            case STDERR_INDEX:
                pollable.fd      = STDERR_FILENO;
                pollable.events  = 0;
                pollable.revents = 0;
                break;
            default:
                pollable.fd      = -1;
                pollable.events  = 0;
                pollable.revents = 0;
                break;
        }
        ++index;
    }
}

void Chat::run(const uint32_t addr, const uint16_t port)
{
    auto init = [&]()
    {
        _server.create();
        _server.set_reuseaddr(true);
        _server.bind(addr, port);
        _server.listen(BACKLOG);
        _pollables[SERVER_INDEX].fd      = _server.fd();
        _pollables[SERVER_INDEX].events  = POLLIN;
        _pollables[SERVER_INDEX].revents = 0;
    };

    auto loop = [&]()
    {
        while(_quit == false) {
            const int timeout = 1000;
            const int ready = poll(_pollables.data(), _pollables.size(), timeout);
            if(ready < 0) {
                if (errno == EINTR) {
                    continue;
                }
                throw std::runtime_error("poll() has failed");
            }
            else if(ready == 0) {
                continue;
            }
            int index = 0;
            for(auto& pollable : _pollables) {
                switch(index) {
                    case STDIN_INDEX:
                        on_stdin(pollable);
                        break;
                    case STDOUT_INDEX:
                        on_stdout(pollable);
                        break;
                    case STDERR_INDEX:
                        on_stderr(pollable);
                        break;
                    case SERVER_INDEX:
                        on_server(pollable);
                        break;
                    case CLIENT_INDEX:
                        on_client(pollable);
                        break;
                    default:
                        break;
                }
                if(pollable.revents & (POLLHUP | POLLERR)) {
                    on_error(pollable);
                }
                ++index;
            }
        }
    };

    if(_quit == false) {
        init();
        loop();
    }
}

void Chat::quit()
{
    _server.close();
    _client.close();
    _quit = true;
}

void Chat::on_error(PollFd& pollable)
{
    pollable.fd = -1;
    quit();
}

void Chat::on_stdin(PollFd& pollable)
{
    if(pollable.fd < 0) {
        return;
    }
    if(pollable.revents & POLLIN) {
        char buffer[1024];
        const ssize_t rc = ::read(STDIN_FILENO, buffer, sizeof(buffer));
        if(rc < 0) {
            throw std::runtime_error("read() has failed");
        }
        if(_client.fd() >= 0) {
            _client.send(std::string(buffer, rc));
        }
    }
}

void Chat::on_stdout(PollFd& pollable)
{
    if(pollable.fd < 0) {
        return;
    }
}

void Chat::on_stderr(PollFd& pollable)
{
    if(pollable.fd < 0) {
        return;
    }
}

void Chat::on_server(PollFd& pollable)
{
    if(pollable.fd < 0) {
        return;
    }
    if(pollable.revents & POLLIN) {
        const int fd = _server.accept();
        if(_client.fd() >= 0) {
            const int rc = ::close(fd);
            if(rc < 0) {
                throw std::runtime_error("close() has failed");
            }
            return;
        }
        else {
            _client.reset(fd);
        }
        _pollables[CLIENT_INDEX].fd      = _client.fd();
        _pollables[CLIENT_INDEX].events  = POLLIN;
        _pollables[CLIENT_INDEX].revents = 0;
    }
    if(_server.fd() < 0) {
        pollable.fd = -1;
        quit();
    }
}

void Chat::on_client(PollFd& pollable)
{
    if(pollable.fd < 0) {
        return;
    }
    if(pollable.revents & POLLIN) {
        std::string buffer;
        if(_client.fd() >= 0) {
            _client.recv(buffer);
        }
        std::cout << buffer;
    }
    if(_client.fd() < 0) {
        pollable.fd = -1;
        quit();
    }
}

void signal_handler(int signum)
{
    printf("Signal %d received \n", signum);
}

void install_signal_handler(int signum) 
{
    struct sigaction sa = {};
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigaction(signum,&sa,NULL);

}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const uint32_t addr = INADDR_ANY;
    const uint16_t port = 10000;

    try {

        install_signal_handler(SIGUSR1);
        install_signal_handler(SIGUSR2);

        Chat chat;

        chat.run(addr, port);
    }
    catch(const std::exception& e) {
        const char* what(e.what());
        std::cerr << "error: " << what << std::endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        const char* what("unhandled exception");
        std::cerr << "error: " << what << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
