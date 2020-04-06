#include "Anywhere_Door.h"

int main()
{
	fmt::print("What is your role:\n");
	fmt::print("1. Sender.\n");
	fmt::print("2. Receiver.\n");

	int choice;
	std::cin >> choice;
	std::cin.get();

	asio::io_context io_context;
	std::thread io_thread;;
	std::unique_ptr<Sender> sender;
	std::unique_ptr<Receiver> receiver;

	try
	{
		// Prevent io_context from returning when there is no more work
		// This is nessesary if the run() is called from the background thread before the async process is started
		asio::executor_work_guard<asio::io_context::executor_type> work_guard = asio::make_work_guard(io_context);

		io_thread = std::thread([&io_context] {
			io_context.run();
			}
		);

		if (choice == 1) {
			sender = std::make_unique<Sender>(io_context);
			sender->start("127.0.0.1", 9985);
		}
		else if (choice == 2) {
			receiver = std::make_unique<Receiver>(io_context, 9985);
		}

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	io_thread.join();

	std::getchar();

	return 0;
}
