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
#include <thread>

class Sender
{
public:
	Sender(asio::io_context& io_context, unsigned int port);
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
	void init_broadcast_listener();
	std::string get_address();
private:
	std::string address_;
	unsigned int port_;
	asio::ip::tcp::socket socket_;
	asio::ip::tcp::endpoint endpoint_;
	flexbuffers::Builder flexbuffers_builder_;
	std::string delim{ "^_^" };
	std::vector<unsigned char> sendBuf_;
	asio::ip::udp::socket broadcast_listener_;
	std::thread broadcast_listen_thread_;
};