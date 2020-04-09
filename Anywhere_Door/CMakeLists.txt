cmake_minimum_required (VERSION 3.15)

add_executable (Anywhere_Door "Anywhere_Door.cpp" "Anywhere_Door.h" "Receiver.cpp" "Sender.cpp")

set_target_properties(Anywhere_Door PROPERTIES CXX_STANDARD 17)

target_compile_definitions(Anywhere_Door PRIVATE ASIO_STANDALONE)

find_package(asio CONFIG REQUIRED)
target_link_libraries(Anywhere_Door PRIVATE asio asio::asio)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(Anywhere_Door PRIVATE fmt::fmt fmt::fmt-header-only)

find_package(Flatbuffers CONFIG REQUIRED)
target_link_libraries(Anywhere_Door PRIVATE flatbuffers::flatbuffers)

if (UNIX)
	target_link_libraries(Anywhere_Door PRIVATE stdc++fs pthread)
endif (UNIX)

if (MSVC)
	target_compile_definitions(Anywhere_Door PRIVATE _WIN32_WINNT=0x0A00)
endif (MSVC)
