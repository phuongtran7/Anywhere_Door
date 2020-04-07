#include "Receiver.h"

session::session(asio::ip::tcp::socket socket)
	: socket_(std::move(socket))
{
}

void session::start()
{
	do_read();
}

void session::do_read()
{
	auto self(shared_from_this());

	asio::async_read_until(socket_, asio::dynamic_buffer(input_buffer_), delim, [this, self](const std::error_code& error, std::size_t bytes_transferred)
		{
			if (!error)
			{
				// Extract the delimiter from the buffer.
				std::string line(input_buffer_.substr(0, bytes_transferred - delim.size()));
				input_buffer_.clear();

				// Empty messages are heartbeats and so ignored.
				if (!line.empty())
				{
					fmt::print("Received: {} bytes.\n", line.size());
					auto data = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(line.data()), line.size()).AsMap();

					auto file_name = data["name"].AsString().c_str();
					auto file_content = data["data"].AsString();

					fmt::print("File name: {}\n", file_name);

					std::ofstream ofile(file_name, std::ios::binary);
					ofile.write(reinterpret_cast<const char*>(file_content.str().data()), file_content.size());
					ofile.close();
				}

				do_read();
			}
			else if (error == asio::error::eof)
			{
				// There is no more message from this connection. It's closed
			}
			else {
				fmt::print("Error on receive: {}\n", error.message());
			}
		}
	);
}

Receiver::Receiver(asio::io_context& io_context, short port)
	: acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
	do_accept();
}

void Receiver::do_accept()
{
	acceptor_.async_accept(
		[this](std::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::make_shared<session>(std::move(socket))->start();
			}

			do_accept();
		});
}