#pragma once

#include <asio.hpp>
#include <memory>
#include <system_error>
#include <string>
#include <fmt/format.h>
#include <flatbuffers/flexbuffers.h>
#include <fstream>
#include <thread>
#include <atomic>

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(asio::ip::tcp::socket socket);
    void start();

private:
    void do_read();

private:
    std::string input_buffer_;
    asio::ip::tcp::socket socket_;
    std::string delim{ "^_^" };
};

class Receiver
{
public:
    Receiver(asio::io_context& io_context, short port);
    ~Receiver();

private:
    void do_accept();
    void broadcast_address();

private:
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::udp::socket broadcaster_;
    asio::ip::udp::endpoint broadcast_destination_;
    std::thread broadcast_thread_;
    std::atomic_bool should_broadcast_;
};
