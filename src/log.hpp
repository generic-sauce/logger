#ifndef __CLASS_LOGGER__
#define __CLASS_LOGGER__

#include <ostream>
#include <vector>
#include <string>

namespace log
{
	struct FormatState {
		std::string seperator;
		bool endlines;
	};

	enum Format {
		reset,
		noseperator,
		endlines,
		noendlines
	};

	class seperator {
		public:
			seperator(std::string str)
				: m_str(std::move(str))
			{}
			std::string m_str;
	};

	class Forwarder
	{
		public:
			Forwarder(const std::vector<std::ostream*>& ostreams, FormatState& format)
				: m_counter(0), m_ostreams(ostreams), m_formatstate(format)
			{
			}

			~Forwarder()
			{
				if (m_formatstate.endlines)
					for (auto* ostream : m_ostreams)
						*ostream << "\n";
			}

			template <class T>
			Forwarder&& operator<<(T&& t)
			{
				for (auto* ostream : m_ostreams)
				{
					printSeperator(*ostream);
					*ostream << std::forward<T>(t);
				}

				m_counter++;
				return std::move(*this);
			}

			Forwarder&& operator<<(Format format)
			{
				switch (format) {
					case Format::reset:
						m_formatstate.seperator.clear();
						m_formatstate.endlines = false;
						break;
					case Format::noseperator:
						m_formatstate.seperator.clear();
						break;
					case Format::endlines:
						m_formatstate.endlines = true;
						break;
					case Format::noendlines:
						m_formatstate.endlines = false;
						break;
				}

				return std::move(*this);
			}

			Forwarder&& operator<<(seperator sep)
			{
				m_formatstate.seperator = sep.m_str;
				return std::move(*this);
			}

			/*Forwarder&& operator<<(
					std::ostream& (*f) (std::ostream&))
			{
				for (auto* ostream : m_ostreams)
					*ostream << f;

				return std::move(*this);
			}*/

		private:
			std::size_t m_counter;
			const std::vector<std::ostream*>& m_ostreams;
			FormatState& m_formatstate;

			inline void printSeperator(std::ostream& ostream)
			{
				if (m_counter > 0 && m_formatstate.seperator.length() > 0)
					ostream << m_formatstate.seperator;
			}
	};

	class Logger
	{
		public:
			void add_stream(std::ostream* ostream)
			{
				m_ostreams.push_back(ostream);
			}

			template <typename ...Ts>
			inline Forwarder operator()(Ts&&... local_formats)
			{
				return Forwarder(m_ostreams, m_formatstate);
			}

		private:
			std::vector<std::ostream*> m_ostreams;
			FormatState m_formatstate;
	};

	Logger out;
	Logger err;
}

#endif
