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
	Sender(asio::io_context& io_context, std::string address, unsigned int port);
	~Sender();

	Sender(const Sender& other) = delete;
	Sender& operator=(const Sender& other) = delete;
	Sender(Sender&& other) noexcept;
	Sender& operator=(Sender&& other) noexcept;

private:
	std::string get_file_list();
	void make_buffer();
	void start_connect();
	void clear_buffer();

private:
	std::string address_;
	unsigned int port_;
	asio::ip::tcp::socket socket_;
	flexbuffers::Builder flexbuffers_builder_;
	std::string delim{ "^_^" };
	std::vector<unsigned char> sendBuf_;
};