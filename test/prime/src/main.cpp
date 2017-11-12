#include <iostream>

#include <log.hpp>

int main()
{
	log::out.add_stream(&std::cout);

	log::out() << log::separator("lll");
	log::out(log::separator(" - "), log::endlines) << "Hello" << "world!";
	return 0;
}
