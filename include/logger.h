#ifndef LOGGER_H
#define LOGGER_H

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/logger.hpp>

#include <boost/log/sinks/text_file_backend.hpp>

#define LOG(X) BOOST_LOG_SEV(Logger::logger_, X)

enum severity_level {
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

// code source: https://www.boost.org/doc/libs/1_54_0/libs/log/doc/html/log/detailed/expressions.html
// The operator is used for regular stream formatting
inline std::ostream& operator<< (std::ostream& strm, severity_level level)
{
    static const char* strings[] =
    {
        "trace",
        "debug",
        "info",
        "warning",
        "error",
        "fatal"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

class Logger {
    public:
        static void initLogger();
        static boost::log::sources::severity_logger_mt<severity_level> getLogger();
        static boost::log::sources::severity_logger_mt<severity_level> logger_;
};

#endif