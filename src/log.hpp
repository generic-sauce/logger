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

	inline void update_format_state(Format format, FormatState* formatstate)
	{
		switch (format) {
			case Format::reset:
				formatstate->seperator.clear();
				formatstate->endlines = false;
				break;
			case Format::noseperator:
				formatstate->seperator.clear();
				break;
			case Format::endlines:
				formatstate->endlines = true;
				break;
			case Format::noendlines:
				formatstate->endlines = false;
				break;
		}

	}

	class Forwarder
	{
		public:
			Forwarder(const std::vector<std::ostream*>& ostreams, FormatState* global_format, FormatState local_format)
				: m_counter(0), m_ostreams(ostreams), m_global_formatstate(global_format), m_local_formatstate(local_format)
			{
			}

			~Forwarder()
			{
				if (m_local_formatstate.endlines)
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
				update_format_state(format, &m_local_formatstate);
				update_format_state(format, m_global_formatstate);
				return std::move(*this);
			}

			Forwarder&& operator<<(seperator sep)
			{
				m_local_formatstate.seperator = sep.m_str;
				m_global_formatstate->seperator = sep.m_str;
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
			FormatState* m_global_formatstate;
			FormatState m_local_formatstate;

			inline void printSeperator(std::ostream& ostream)
			{
				if (m_counter > 0 && m_local_formatstate.seperator.length() > 0)
					ostream << m_local_formatstate.seperator;
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
			inline void recursive_format_check(FormatState* formatstate, Format format, Ts... local_formats)
			{
				update_format_state(format, formatstate);
				recursive_format_check(formatstate, local_formats...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, seperator sep, Ts... local_formats)
			{
				formatstate->seperator = sep.m_str;
				recursive_format_check(formatstate, local_formats...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState*) {}

			template <typename ...Ts>
			inline Forwarder operator()(Ts&&... local_formats)
			{
				if constexpr (sizeof...(Ts) > 0)
				{
					FormatState local_state = m_global_formatstate;;
					recursive_format_check(&local_state, local_formats...);
					return Forwarder(m_ostreams, &m_global_formatstate, local_state);
				}
				else
					return Forwarder(m_ostreams, &m_global_formatstate, m_global_formatstate);
			}

		private:
			std::vector<std::ostream*> m_ostreams;
			FormatState m_global_formatstate;
	};

	Logger out;
	Logger err;
}

#endif
