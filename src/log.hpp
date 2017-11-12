#ifndef __CLASS_LOGGER__
#define __CLASS_LOGGER__

#include <ostream>
#include <vector>
#include <string>

namespace log
{
	struct FormatState {
		std::string separator;
		bool endlines;
	};

	enum FormatModifier {
		reset,
		noseparator,
		endlines,
		noendlines
	};

	class separator {
		public:
			separator(std::string str)
				: m_str(std::move(str))
			{}
			std::string m_str;
	};

	inline void update_format_state(FormatModifier format_mod, FormatState* formatstate)
	{
		switch (format_mod) {
			case FormatModifier::reset:
				formatstate->separator.clear();
				formatstate->endlines = false;
				break;
			case FormatModifier::noseparator:
				formatstate->separator.clear();
				break;
			case FormatModifier::endlines:
				formatstate->endlines = true;
				break;
			case FormatModifier::noendlines:
				formatstate->endlines = false;
				break;
		}

	}

	class Forwarder
	{
		public:
			Forwarder(const std::vector<std::ostream*>& ostreams, FormatState* global_format, FormatState local_format)
				: m_counter(0), m_ostreams(ostreams), m_global_formatstate(global_format), m_local_formatstate(local_format)
			{}

			~Forwarder()
			{
				if (m_local_formatstate.endlines)
					for (auto* ostream : m_ostreams)
						*ostream << '\n';
			}

			template <class T>
			Forwarder&& operator<<(T&& t)
			{
				for (auto* ostream : m_ostreams)
				{
					print_separator(ostream);
					*ostream << std::forward<T>(t);
				}

				m_counter++;
				return std::move(*this);
			}

			Forwarder&& operator<<(FormatModifier format_mod)
			{
				update_format_state(format_mod, &m_local_formatstate);
				update_format_state(format_mod, m_global_formatstate);
				return std::move(*this);
			}

			Forwarder&& operator<<(separator sep)
			{
				m_local_formatstate.separator = sep.m_str;
				m_global_formatstate->separator = sep.m_str;
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

			inline void print_separator(std::ostream* ostream)
			{
				if (m_counter > 0 && !m_local_formatstate.separator.empty())
					*ostream << m_local_formatstate.separator;
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
			inline void recursive_format_check(FormatState* formatstate, FormatModifier format_mod, Ts... local_formats)
			{
				update_format_state(format_mod, formatstate);
				recursive_format_check(formatstate, local_formats...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, separator sep, Ts... local_formats)
			{
				formatstate->separator = sep.m_str;
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
