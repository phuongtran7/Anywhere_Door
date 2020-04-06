#pragma once

#include <asio.hpp>
#include <memory>
#include <system_error>
#include <string>
#include <fmt/format.h>
#include <flatbuffers/flexbuffers.h>
#include <fstream>

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    static std::shared_ptr<tcp_connection> create(asio::io_context& io_context);
    asio::ip::tcp::socket& socket();
    void start_read();

private:
    tcp_connection(asio::io_context& io_context);

private:
    asio::ip::tcp::socket socket_;
    std::string input_buffer_;
};

class Receiver {
public:
    Receiver(asio::io_context& io_context);

private:
    void start_accept();
private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};