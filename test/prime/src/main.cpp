#include <iostream>

#include <log.hpp>

int main()
{
	log::out.add_stream(&std::cout);

	log::out() << log::seperator("lll");
	log::out(log::seperator(" - "), log::endlines) << "Hello" << "world!";
	return 0;
}
