#ifndef __CLASS_LOGGER__
#define __CLASS_LOGGER__

#include <ostream>
#include <vector>

#include <OutStream.hpp>

namespace log
{
	class Logger
	{
		public:
			void addStream(OutStream stream);

			template <class T>
			Logger& operator<<(T&& t)
			{
				for (OutStream stream : streams)
				{
					stream << std::forward<T>(t);
				}
				return *this;
			}

			Logger& operator<<(
					std::basic_ostream<char, std::char_traits<char>>&
					(*f)
					(std::basic_ostream<char, std::char_traits<char>>&))
			{
				for (OutStream stream : streams)
				{
					stream << f;
				}
				return *this;
			}

		private:
			std::vector<OutStream> streams;
	};
}

#endif
