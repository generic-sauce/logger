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

      void operator<<(
          std::basic_ostream<char, std::char_traits<char>>&
          (*f)
          (std::basic_ostream<char, std::char_traits<char>>&))
      {
				(*stream) << f;
      }

			void setSeparator(const std::string& sep);
		private:
			std::ostream *stream;
			std::string separator;
	};
}

#endif
