#include "byte_stream.hh"
using namespace std;

ByteStream::ByteStream(uint64_t capacity)
    : buffer_(),
      capacity_(capacity),
      bytes_pushed_(0),
      bytes_popped_(0),
      closed_(false),
      has_error_(false) {}


//the methods for writer functionality
void Writer::push(string data)
{
    uint64_t space = available_capacity();
    uint64_t n = min(space, (uint64_t)data.size());

    for (uint64_t i = 0; i < n; i++)
        buffer_.push_back(data[i]);

    bytes_pushed_ += n;
}

void Writer::close()
{
    closed_ = true;
}

bool Writer::is_closed() const
{
    return closed_;
}

uint64_t Writer::available_capacity() const
{
    return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
    return bytes_pushed_;
}

//the methods for the reader functionality
string_view Reader::peek() const
{
    if (buffer_.empty()) return {};
    return string_view(&buffer_.front(), 1);
}

void Reader::pop(uint64_t len)
{
    uint64_t n = min(len, (uint64_t)buffer_.size());

    for (uint64_t i = 0; i < n; i++)
        buffer_.pop_front();

    bytes_popped_ += n;
}

bool Reader::is_finished() const
{
    return closed_ && buffer_.empty();
}

uint64_t Reader::bytes_buffered() const
{
    return buffer_.size();
}

uint64_t Reader::bytes_popped() const
{
    return bytes_popped_;
}
