#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <log.hpp>

bool is_prime(std::size_t number)
{
	for (std::size_t i = 2; i < number; i++)
		if (number % i == 0)
			return false;

	return true;
}

int main()
{
	log::out.add_stream(&std::cout);
	log::err.add_stream(&std::cerr);

	log::out() << log::separator(" ") << log::endlines;
	log::err() << log::separator(" ") << log::endlines;

	const std::size_t begin = 0;
	const std::size_t end = 10;

	for (std::size_t i = begin; i < end; i++)
	{
		if (is_prime(i))
			log::out() << i << "is a prime.";
		else
			log::err() << i << "is not a prime";
	}

	return 0;
}
