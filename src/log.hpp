#ifndef __CLASS_LOGGER__
#define __CLASS_LOGGER__

#include <ostream>
#include <vector>
#include <string>
#include <algorithm>

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
		noendlines,
		exclude,
		easecase,
		lowercase,
		uppercase,
	};

	class separator {
		public:
			separator(std::string str) : m_str(std::move(str)) {}
			std::string str() { return m_str; }
		private:
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
				if (m_counter > 0)
					if (m_local_formatstate.endlines)
						for (auto* ostream : m_ostreams)
							*ostream << '\n';
			}

			template <typename T>
			Forwarder&& operator<<(T&& t)
			{
				for (auto* ostream : m_ostreams)
				{
					print_separator(ostream);

					if constexpr (
							std::is_same<
								typename std::remove_reference<T>::type,
								std::string>::value)
					{
						*ostream << [](std::string s){ return s; }();
					}
					else
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
				m_local_formatstate.separator = sep.str();
				m_global_formatstate->separator = sep.str();
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
			void clear_streams()
			{
				m_ostreams.clear();
			}

			bool has_stream(std::ostream* ostream)
			{
				return std::find(std::begin(m_ostreams), std::end(m_ostreams), ostream) != std::end(m_ostreams);
			}

			void add_stream(std::ostream* ostream)
			{
				m_ostreams.push_back(ostream);
			}

			void remove_stream(std::ostream* ostream)
			{
				m_ostreams.erase(
						std::remove(std::begin(m_ostreams), std::end(m_ostreams), ostream),
						std::end(m_ostreams));
			}

		private:
			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, FormatModifier format_mod, Ts... local_formats)
			{
				update_format_state(format_mod, formatstate);
				recursive_format_check(formatstate, local_formats...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, separator sep, Ts... local_formats)
			{
				formatstate->separator = sep.str();
				recursive_format_check(formatstate, local_formats...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState*) {}

		public:
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

	static Logger out;
	static Logger err;
}

#endif
