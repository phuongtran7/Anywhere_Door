#include "Sender.h"

Sender::Sender(asio::io_context& io_context, unsigned int port) :
	address_{},
	port_(port),
	socket_(io_context),
	endpoint_(),
	broadcast_listener_(io_context)
{
	init_broadcast_listener();
	address_ = get_address();
	if (!address_.empty()) {
		fmt::print("Got receiver address: {}\n", address_);
		endpoint_ = asio::ip::tcp::endpoint(asio::ip::address::from_string(address_), port_);
		start_connect();
	}
}

Sender::~Sender()
{
	std::error_code ec;
	socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket_.close();
	clear_buffer();
}

Sender::Sender(Sender&& other) noexcept :
	address_(std::exchange(other.address_, {})),
	port_(std::exchange(other.port_, 0)),
	socket_(std::move(other.socket_)),
	endpoint_(std::exchange(other.endpoint_, {})),
	broadcast_listener_(std::move(other.broadcast_listener_))
{
	init_broadcast_listener();
	address_ = get_address();
	if (!address_.empty()) {
		fmt::print("Got receiver address: {}\n", address_);
		endpoint_ = asio::ip::tcp::endpoint(asio::ip::address::from_string(address_), port_);
		start_connect();
	}
}

Sender& Sender::operator=(Sender&& other) noexcept
{
	std::swap(address_, other.address_);
	std::swap(port_, other.port_);
	std::swap(socket_, other.socket_);
	std::swap(endpoint_, other.endpoint_);
	std::swap(broadcast_listener_, other.broadcast_listener_);

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

	std::ifstream file(file_name, std::ios::binary);
	std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

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
	socket_.async_connect(endpoint_, handle_connect);
}

void Sender::clear_buffer()
{
	sendBuf_.clear();
	flexbuffers_builder_.Clear();
}

void Sender::init_broadcast_listener()
{
	auto endpoint = asio::ip::udp::endpoint(asio::ip::udp::v4(), 55987);
	broadcast_listener_.open(endpoint.protocol());
	broadcast_listener_.set_option(asio::ip::udp::socket::reuse_address(true));
	broadcast_listener_.bind(endpoint);
	broadcast_listener_.set_option(asio::socket_base::broadcast(true));
}

std::string Sender::get_address()
{
	asio::ip::udp::endpoint sender_endpoint;
	std::array<char, 50> buffer;
	auto byte_recv = broadcast_listener_.receive_from(asio::buffer(buffer), sender_endpoint);
	if (byte_recv != 0) {
		if (std::string(buffer.data(), byte_recv) == "I Am Here") {
			return sender_endpoint.address().to_string();
		}
	}
	return std::string{};
}
