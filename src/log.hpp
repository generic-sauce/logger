#ifndef __CLASS_LOGGER__
#define __CLASS_LOGGER__

#include <ostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <type_traits>
#include <cstring>

namespace {
void toLowerCase(char* cstr)
{
	for (char* c = cstr; *c != '\0'; c++)
		if (*c >= 'A' && *c <= 'Z')
			*c += 'a'-'A';
}

void toUpperCase(char* cstr)
{
	for (char* c = cstr; *c != '\0'; c++)
		if (*c >= 'a' && *c <= 'z')
			*c += 'A'-'a';
}
} // unnamed namespace

namespace log
{
	enum FormatFlags {
		reset,
		noseparator,
		endlines,
		noendlines,
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

	struct FormatState {
		std::string separator;
		bool endlines = false;
		FormatFlags lettercase = FormatFlags::easecase;
	};

	inline void update_format_state(FormatFlags format_mod, FormatState* formatstate)
	{
		switch (format_mod) {
			case FormatFlags::reset:
				formatstate->separator.clear();
				formatstate->endlines = false;
				formatstate->lettercase = FormatFlags::easecase;
				break;
			case FormatFlags::noseparator:
				formatstate->separator.clear();
				break;
			case FormatFlags::endlines:
				formatstate->endlines = true;
				break;
			case FormatFlags::noendlines:
				formatstate->endlines = false;
				break;
			case FormatFlags::easecase:
				formatstate->lettercase = FormatFlags::easecase;
				break;
			case FormatFlags::lowercase:
				formatstate->lettercase = FormatFlags::lowercase;
				break;
			case FormatFlags::uppercase:
				formatstate->lettercase = FormatFlags::uppercase;
				break;
		}
	}

	class Forwarder
	{
		public:
			Forwarder(const std::vector<std::ostream*>& ostreams,
					FormatState* global_format,
					FormatState local_format)
				: m_counter(0), m_ostreams(ostreams)
				, m_global_formatstate(global_format)
				, m_local_formatstate(local_format)
			{}

			Forwarder() = delete;
			Forwarder(const Forwarder&) = delete;
			Forwarder(Forwarder&&) = delete;

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
					if constexpr(std::is_convertible_v<std::remove_reference_t<T>, std::string_view>)
					{
						std::string_view view(t);
						char cstr[view.length() + 1];
						std::strcpy(cstr, view.data());
						cstr[view.length()] = '\0';
						switch (m_local_formatstate.lettercase)
						{
							case FormatFlags::lowercase:
								toLowerCase(cstr);
								break;
							case FormatFlags::uppercase:
								toUpperCase(cstr);
								break;
							default:
								break;
						}
						*ostream << cstr;
					}
					else
						*ostream << std::forward<T>(t);
				}

				m_counter++;
				return std::move(*this);
			}

			Forwarder&& operator<<(FormatFlags format_mod)
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

			Forwarder&& operator<<(std::ostream& (*f) (std::ostream&))
			{
				for (auto* ostream : m_ostreams) 
					*ostream << f;

				return std::move(*this);
			}

			Forwarder&& operator<<(std::basic_ios<char, std::char_traits<char>>& (*f) (std::basic_ios<char, std::char_traits<char>>&))
			{
				for (auto* ostream : m_ostreams)
					*ostream << f;

				return std::move(*this);
			}

			Forwarder&& operator<<(std::ios_base& (*f) (std::ios_base&))
			{
				for (auto* ostream : m_ostreams)
					*ostream << f;

				return std::move(*this);
			}

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

			void setstdf(std::ios::fmtflags fmtflags)
			{
				for (auto* ostream : m_ostreams)
					ostream->setf(fmtflags);
			}

			void unsetstdf(std::ios::fmtflags fmtflags)
			{
				for (auto* ostream : m_ostreams)
					ostream->unsetf(fmtflags);
			}

		private:
			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, FormatFlags flag, Ts... rest)
			{
				update_format_state(flag, formatstate);
				recursive_format_check(formatstate, rest...);
			}

			template <typename ...Ts>
			inline void recursive_format_check(FormatState* formatstate, separator sep, Ts... rest)
			{
				formatstate->separator = sep.str();
				recursive_format_check(formatstate, rest...);
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
