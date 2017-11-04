#include <iostream>

#include <Logger.hpp>

int main(int argc, char* argv[])
{
	log::Logger logger;
	logger.addStream(log::OutStream(&std::cout, " hihi "));
	logger << "hello" << "World!" << std::endl;
	return 0;
}
