#ifndef __OUTSTREAM_CLASS__
#define __OUTSTREAM_CLASS__

#include <ostream>

namespace log
{
	class OutStream
	{
		public:
			OutStream(std::ostream* stream);
			OutStream(std::ostream* stream, const std::string& sep);

			template <typename T>
			void operator<<(T&& t)
			{
				(*stream) << t << separator;
			}

			void setSeparator(const std::string& sep);
		private:
			std::ostream *stream;
			std::string separator;
	};
}

#endif
