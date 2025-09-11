#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <cstdint>

class Reader;
class Writer;

class ByteStream
{
private:
    std::deque<char> buffer_;
    uint64_t capacity_;
    uint64_t bytes_pushed_ = 0;
    uint64_t bytes_popped_ = 0;
    bool closed_ = false;
    bool has_error_ = false;


public:
    ByteStream(uint64_t capacity);

    Reader& reader();
    const Reader& reader() const;
    Writer& writer();
    const Writer& writer() const;

    void set_error() { has_error_ = true; }
bool has_error() const { return has_error_; }


    friend class Reader;
    friend class Writer;
};

class Writer : public ByteStream
{
public:
    using ByteStream::ByteStream;

    void push(std::string data);
    void close();
    bool is_closed() const;
    uint64_t available_capacity() const;
    uint64_t bytes_pushed() const;
};

class Reader : public ByteStream
{
public:
    using ByteStream::ByteStream;

    std::string_view peek() const;
    void pop(uint64_t len);
    bool is_finished() const;
    uint64_t bytes_buffered() const;
    uint64_t bytes_popped() const;
};
