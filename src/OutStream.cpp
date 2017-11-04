#include "OutStream.hpp"

log::OutStream::OutStream(std::ostream* stream)
	: stream(stream), separator("")
{}
 
log::OutStream::OutStream(std::ostream* stream, const std::string& sep)
	: stream(stream), separator(sep)
{} 

void log::OutStream::setSeparator(const std::string& sep)
{
	separator = sep;
}
