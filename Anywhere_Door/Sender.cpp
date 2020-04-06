#include "Sender.h"

Sender::Sender(asio::io_context& io_context)
    : socket_(io_context),
	flexbuffers_builder_(512)
{
}

void Sender::start(const std::string& address, unsigned int port)
{
    start_connect(address, port);
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
	std::basic_ifstream<uint8_t> fStream{ file_name, std::ios::binary };
	//std::vector<uint8_t> file_content{ std::istreambuf_iterator<uint8_t>(fStream), {} };
	std::string file_content{ std::istreambuf_iterator<uint8_t>(fStream), {} };

	auto map_start = flexbuffers_builder_.StartMap();
	flexbuffers_builder_.String("name", file_name.c_str());
	flexbuffers_builder_.String("data", file_content.data());
	flexbuffers_builder_.EndMap(map_start);
	flexbuffers_builder_.Finish();
}

void Sender::start_connect(const std::string& address, unsigned int port)
{
	auto handle_write = [&](const std::error_code& ec, std::size_t bytes_transferred) {
		if (!ec) {
			// Write the delimnator to signal the end of message.
			asio::write(socket_, asio::buffer("\r\n", 3));
		}
		else {
			fmt::print("Write error: {}\n", ec.message());
		}
	};

	auto handle_connect = [&](const std::error_code& ec) {
		if (!ec) {
			make_buffer();
			// Start an asynchronous operation to write the message.
			asio::async_write(socket_, asio::buffer(flexbuffers_builder_.GetBuffer(), flexbuffers_builder_.GetSize()), handle_write);
		}
		else {
			fmt::print("Connect error: {}\n", ec.message());
		}
	};

	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(address), port);
	// Start the asynchronous connect operation.
	socket_.async_connect(endpoint, handle_connect);

	//socket_.connect(endpoint);
	//make_buffer();
	//asio::write(socket_, asio::buffer(flexbuffers_builder_.GetBuffer(), flexbuffers_builder_.GetSize()));
	//asio::write(socket_, asio::buffer("\r\n", 3));
}