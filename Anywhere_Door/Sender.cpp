#include "Sender.h"

Sender::Sender(asio::io_context& io_context, std::string address, unsigned int port)
    : address_(std::move(address)),
	port_(port),
	socket_(io_context)
{
	start_connect();
}

Sender::~Sender()
{
	clear_buffer();
}

Sender::Sender(Sender&& other) noexcept :
	address_(std::exchange(other.address_, {})),
	port_(std::exchange(other.port_, 0)),
	socket_(std::move(other.socket_))
{
	start_connect();
}

Sender& Sender::operator=(Sender&& other) noexcept
{
	std::swap(address_, other.address_);
	std::swap(port_, other.port_);
	std::swap(socket_, other.socket_);
	return *this;
}

std::string Sender::get_file_list()
{
	std::vector<std::string> files{};
	auto current_path = std::filesystem::current_path();
	for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
		files.push_back(entry.path().filename().string());
	}

	if (!files.empty()) {
		if (files.size() > 1) {
			fmt::print("Files discovered. Please choose one: \n");
			for (auto i = 0; i < files.size(); i++) {
				fmt::print("{} - {}\n", i, files.at(i));
			}

			int choice;
			std::cin >> choice;
			std::cin.get();
			return files.at(choice);
		}
		else {
			return files.at(0);
		}
	}
	else {
		fmt::print("Folder is empty.\n");
		return "";
	}
}

void Sender::make_buffer()
{
	auto file_name = get_file_list();

	if (file_name.empty()) {
		return;
	}
	std::basic_ifstream<unsigned char> fStream{ file_name, std::ios::binary };
	std::string file_content{ std::istreambuf_iterator<unsigned char>(fStream), {} };

	auto map_start = flexbuffers_builder_.StartMap();
	flexbuffers_builder_.String("name", file_name.c_str());
	flexbuffers_builder_.String("data", file_content.data());
	flexbuffers_builder_.EndMap(map_start);
	flexbuffers_builder_.Finish();

	// Append the deliminator
	sendBuf_ = flexbuffers_builder_.GetBuffer();
	sendBuf_.insert(sendBuf_.end(), delim.begin(), delim.end());
}

void Sender::start_connect()
{
	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(address_), port_);

	auto handle_connect = [&](const std::error_code& ec) {
		if (!ec) {
			make_buffer();

			// Start an asynchronous operation to write the message.
			asio::async_write(socket_, asio::buffer(sendBuf_), [&](const std::error_code& ec, std::size_t bytes_transferred) {
				if (!ec) {
					fmt::print("Sent: {} bytes.\n", bytes_transferred);
				}
				else {
					fmt::print("Write error: {}\n", ec.message());
				}
				});
		}
		else {
			fmt::print("Connect error: {}\n", ec.message());
		}
	};

	// Start the asynchronous connect operation.
	socket_.async_connect(endpoint, handle_connect);
}

void Sender::clear_buffer()
{
	sendBuf_.clear();
	flexbuffers_builder_.Clear();
}
