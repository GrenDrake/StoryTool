#include <string>

#ifdef BOOST

#include <boost/regex.hpp>

std::string doReplace(const std::string &source, const std::string &pattern,
					  const std::string &result) {

	boost::regex regexPattern(pattern);
	return boost::regex_replace(source, regexPattern, result);
}

#else

#include <regex>

std::string doReplace(const std::string &source, const std::string &pattern,
					  const std::string &result) {

	std::regex regexPattern(pattern);
	return std::regex_replace(source, regexPattern, result);
}

#endif
