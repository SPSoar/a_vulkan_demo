#ifndef S_SPBINARY_H_
#define S_SPBINARY_H_

#include "SPVariable.h"

#include <fstream>
#include <iostream>

class SPBinaryInput
{
public:
    virtual ~SPBinaryInput() = default;
    virtual sp_uint_64 open() noexcept = 0;
    virtual sp_uchar* read() noexcept = 0;
    virtual sp_bool close() noexcept = 0;
    virtual const sp_char* get_name() const noexcept = 0;
};

class SPBinaryOutput
{
public:
    virtual ~SPBinaryOutput() = default;
    virtual sp_bool open() noexcept = 0;
    virtual sp_bool write(const sp_uint_8* data, sp_uint_64 size) noexcept = 0;
    virtual std::ostream& get_output_stream() noexcept = 0;
    virtual sp_bool close() noexcept = 0;
};



class SPFileInput : public SPBinaryInput
{
public:
    explicit SPFileInput(const sp_char* path) 
        : path_(path) 
        , file_size_(0)
    {
    }

    virtual ~SPFileInput() = default;
    sp_uint_64 open() noexcept override;
    sp_uchar* read()  noexcept override;
    sp_bool close() noexcept override;
    const sp_char* get_name() const noexcept override;

private:
    const sp_string path_;
    std::ifstream file_;
    sp_uint_64 file_size_;
};


class FileOutput : public SPBinaryOutput
{
public:
    explicit FileOutput(const sp_char* path) 
        : path_(path)
    {
    }

    virtual ~FileOutput() = default;
    sp_bool open() noexcept override;
    sp_bool write(const sp_uint_8* data, sp_uint_64 size) noexcept override;
    std::ostream& get_output_stream() noexcept override;
    sp_bool close() noexcept override;
private:
    const sp_string path_;
    std::ofstream file_;
};


#endif 

