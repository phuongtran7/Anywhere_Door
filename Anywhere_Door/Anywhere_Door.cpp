﻿#include "Anywhere_Door.h"

int main()
{
    try
    {
        asio::io_context io_context;
        Receiver server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
	return 0;
}
