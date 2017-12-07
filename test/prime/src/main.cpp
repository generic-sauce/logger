#include <iostream>
#include <sstream>
#include <log.hpp>

bool is_prime(int v)
{
	if (v <= 2)
		return false;

	for (int i = 2; i <= v/2; i++)
		if (v % i == 0)
			return false;
	return true;
}

int main(int argc, char* argv[])
{
	log::out.add_stream(&std::cout);
	log::err.add_stream(&std::cerr);

	log::out() << log::separator(" ") << log::endlines << std::boolalpha << log::uppercase;
	log::err() << log::separator(" ") << log::endlines << std::boolalpha << log::lowercase;

	std::size_t begin = 0;
	std::size_t end = 10;
	if (argc == 3) {
		begin = atoi(argv[1]);
		end = atoi(argv[2]);
	} else if (argc == 2) {
		begin = atoi(argv[1]);
		end = begin + 1;
	} else {
		begin = 0;
		end = 10;
	}

	for (std::size_t i = begin; i < end; i++)
		if (is_prime(i))
			log::out() << "It Is" << true  << "That" << i << "Is A Prime";
		else
			log::err() << "It Is" << false << "That" << i << "Is A Prime";

	return 0;
}
