#include "Receiver.h"

std::shared_ptr<tcp_connection> tcp_connection::create(asio::io_context& io_context)
{
	//return std::make_shared<tcp_connection>(io_context);
	return std::shared_ptr<tcp_connection>(new tcp_connection(io_context));
}

asio::ip::tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start_read()
{
	// Start an asynchronous operation to read messages.
	asio::async_read_until(socket_, asio::dynamic_buffer(input_buffer_), "\r\n", [&](const std::error_code& error, size_t bytes_transferred)
		{
			if (!error)
			{
				// Extract the delimiter from the buffer.
				std::string line(input_buffer_.substr(0, bytes_transferred - 2));
				input_buffer_.clear();

				// Empty messages are heartbeats and so ignored.
				if (!line.empty())
				{
					fmt::print("Received: {} bytes.\n", line.size());
				}

				start_read();
			}
			else
			{
				fmt::print("Error on receive: {}\n", error.message());
			}
		}
	);
}

tcp_connection::tcp_connection(asio::io_context& io_context)
	: socket_(io_context)
{
}

Receiver::Receiver(asio::io_context& io_context)
	: io_context_(io_context),
	acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9985))
{
	start_accept();
}

void Receiver::start_accept()
{
	std::shared_ptr<tcp_connection> new_connection = tcp_connection::create(io_context_);

	acceptor_.async_accept(new_connection->socket(), [&](const std::error_code& error)
		{
			if (!error)
			{
				new_connection->start_read();
			}
			start_accept();
		}
	);
}