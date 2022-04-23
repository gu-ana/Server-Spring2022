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

class Logger {
    public:
        static void initLogger();
        static boost::log::sources::severity_logger<severity_level> getLogger();
        static boost::log::sources::severity_logger<severity_level> logger_;
};