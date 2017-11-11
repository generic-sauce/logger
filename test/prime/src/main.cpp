#include <iostream>

#include <log.hpp>

int main()
{
	log::out.add_stream(&std::cout);

	log::out() << log::endlines << log::seperator(" ");
	log::out() << "Helloworld!";
	log::out() << "Hello" << "world!";
	return 0;
}
