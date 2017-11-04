#include "Logger.hpp"

void log::Logger::addStream(OutStream stream)
{
	streams.push_back(stream);
}
