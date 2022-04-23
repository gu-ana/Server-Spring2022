#include "logger.h"

boost::log::sources::severity_logger<severity_level> Logger::logger_;

void Logger::initLogger() 
{
    
    std::string format = "[%TimeStamp%] [%Severity%] Thread %ThreadID%: %Message%";
    boost::log::register_simple_formatter_factory<severity_level, char >("Severity");
    
    boost::log::add_file_log
    (
        boost::log::keywords::file_name = "logs/serverlog_%Y-%m-%d.log",
        boost::log::keywords::rotation_size = 10 * 1024 * 1024, // 10MB file size
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // rotate at midnight
        boost::log::keywords::auto_flush = true,
        boost::log::keywords::format = format
    ); 

    boost::log::add_console_log
    (
      std::cout,
      boost::parameter::keyword<boost::log::keywords::tag::format>::get() = format,
      boost::log::keywords::auto_flush = true
    );

    boost::log::add_common_attributes();
}

boost::log::sources::severity_logger<severity_level> Logger::getLogger() 
{
    return logger_;
}