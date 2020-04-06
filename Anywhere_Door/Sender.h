#pragma once

#include <asio.hpp>
#include <memory>
#include <system_error>
#include <string>
#include <fmt/format.h>
#include <flatbuffers/flexbuffers.h>
#include <filesystem>
#include <iostream>
#include <fstream>

class Sender
{
public:
	Sender(asio::io_context& io_context);
	void start(const std::string& address, unsigned int port);

private:
	std::string get_file_list();
	void make_buffer();
	void start_connect(const std::string& address, unsigned int port);

private:
	asio::ip::tcp::socket socket_;
	flexbuffers::Builder flexbuffers_builder_;
	std::string delim{ "^_^" };
};