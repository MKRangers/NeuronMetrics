#ifndef nmEXCEPTIONS_H
#define nmEXCEPTIONS_H

#include <string>
#include <sstream>
#include <exception>

namespace nm
{

class NM_Exception : public std::exception
{
	public:
				NM_Exception(const std::string& msg) : mMessage(msg) {}
				NM_Exception(const std::stringstream& msg) : mMessage(msg.str()) {}

		virtual	~NM_Exception() noexcept = default;
		
		virtual const char* what() const noexcept { return mMessage.c_str(); }


	protected:
		std::string			mMessage;
};

class FileNotFoundException : public NM_Exception
{
	public:
		FileNotFoundException(const std::string& filePath) : NM_Exception("File not found: " + filePath) {}
		FileNotFoundException(const std::stringstream& msg) : NM_Exception(msg) {}
};

}

#endif