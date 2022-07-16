#ifndef S_SPEXCEPTION_H_
#define S_SPEXCEPTION_H_

#include "SPVariable.h"
#include "SPMacro.h"
#include <exception>
class SPException : public std::exception
{
protected:
    sp_int_64 line;
    const sp_char* typeName;
    sp_string description;
    sp_string source;
    const sp_char* file;
    mutable sp_string fullDesc; 
public:

    enum class ExceptionCodes : sp_uint_8
    {
        ERR_CANNOT_WRITE_TO_FILE = 0,
        ERR_INVALID_STATE,
        ERR_INVALIDPARAMS,
        ERR_RENDERINGAPI_ERROR,
        ERR_DUPLICATE_ITEM,
        ERR_ITEM_NOT_FOUND = ERR_DUPLICATE_ITEM,
        ERR_FILE_NOT_FOUND,
        ERR_INTERNAL_ERROR,
        ERR_RT_ASSERTION_FAILED,
        ERR_NOT_IMPLEMENTED,
        ERR_INVALID_CALL
    };


    SPException(sp_int_32 number, const sp_string& description, const sp_string& source);
    SPException(sp_int_32 number, const sp_string& description, const sp_string& source, const sp_char* type, const sp_char* file, sp_int_64 line);
    SPException(const SPException& rhs);
    ~SPException() throw() {}
    virtual void  send_full_description(void) const;
    virtual const sp_string& get_full_description(void) const;
    const sp_string& get_source() const { return source; }
    const sp_char* get_file() const { return file; }
    sp_int_64 get_line() const { return line; }
    const sp_string& get_description(void) const { return description; }
    const char* what() const throw() { return get_full_description().c_str(); }

};

class UnimplementedException : public SPException
{
public:
    UnimplementedException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class FileNotFoundException : public SPException
{
public:
    FileNotFoundException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class IOException : public SPException
{
public:
    IOException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class InvalidStateException : public SPException
{
public:
    InvalidStateException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class InvalidParametersException : public SPException
{
public:
    InvalidParametersException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class ItemIdentityException : public SPException
{
public:
    ItemIdentityException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class InternalErrorException : public SPException
{
public:
    InternalErrorException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class RenderingAPIException : public SPException
{
public:
    RenderingAPIException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class RuntimeAssertionException : public SPException
{
public:
    RuntimeAssertionException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};
class InvalidCallException : public SPException
{
public:
    InvalidCallException(sp_int_32 inNumber, const sp_string& inDescription, const sp_string& inSource, const sp_char* inFile, sp_int_64 inLine)
        : SPException(inNumber, inDescription, inSource, (sp_char*) __FUNCTION__, inFile, inLine) {}
};

class SPExceptionFactory
{
private:

    SPExceptionFactory() = default;
    static SP_NORETURN void _Throw_Exception(
        SPException::ExceptionCodes code, sp_int_32 number,
        const sp_string& desc,
        const sp_string& src, const sp_char* file, sp_int_64 line)
    {
        switch (code)
        {
        case SPException::ExceptionCodes::ERR_CANNOT_WRITE_TO_FILE:   throw IOException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_INVALID_STATE:          throw InvalidStateException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_INVALIDPARAMS:          throw InvalidParametersException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_RENDERINGAPI_ERROR:     throw RenderingAPIException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_DUPLICATE_ITEM:         throw ItemIdentityException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_FILE_NOT_FOUND:         throw FileNotFoundException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_INTERNAL_ERROR:         throw InternalErrorException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_RT_ASSERTION_FAILED:    throw RuntimeAssertionException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_NOT_IMPLEMENTED:        throw UnimplementedException(number, desc, src, file, line);
        case SPException::ExceptionCodes::ERR_INVALID_CALL:           throw InvalidCallException(number, desc, src, file, line);
        default:                                    throw SPException(number, desc, src, (sp_char*)"SPException", file, line);
        }
    }
public:
    static SP_NORETURN void Throw_Exception(
        SPException::ExceptionCodes code,
        const sp_string& desc,
        const sp_string& src, const sp_char* file, sp_int_64 line)
    {
        _Throw_Exception(code, (sp_int_32) code, desc, src, file, line);
    }
};




#endif
