#pragma once

#include <asio.hpp>
#include <memory>
#include <system_error>
#include <string>
#include <fmt/format.h>
#include <flatbuffers/flexbuffers.h>
#include <fstream>

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
};

class Receiver
{
public:
    Receiver(asio::io_context& io_context, short port);

private:
    void do_accept();
    asio::ip::tcp::acceptor acceptor_;
};
