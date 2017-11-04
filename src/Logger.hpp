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
		private:
			std::vector<OutStream> streams;
	};
}

#endif
