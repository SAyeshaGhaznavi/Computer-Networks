#include "byte_stream.hh"

#include <algorithm>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

// for writer

void Writer::push( string data )
{
// Determine writable bytes (do not exceed available capacity)
uint64_t buf_size = static_cast<uint64_t>(buffer_.size());
uint64_t avail = capacity_ > buf_size ? (capacity_ - buf_size) : 0;
uint64_t want = static_cast<uint64_t>(data.size());
uint64_t writable = std::min(avail, want);

if (writable == 0u) return;

// append only the writable prefix
buffer_.append( data.data(), static_cast<size_t>(writable) );
bytes_pushed_ += writable;
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
uint64_t buf_size = static_cast<uint64_t>(buffer_.size());
return buf_size >= capacity_ ? 0u : (capacity_ - buf_size);
}

uint64_t Writer::bytes_pushed() const
{
return bytes_pushed_;
}

// for reader

string_view Reader::peek() const
{
// Safe: buffer_ is a contiguous std::string, so we can return a string_view into it.
if (buffer_.empty()) return string_view();
return string_view( buffer_.data(), buffer_.size() );
}

void Reader::pop( uint64_t len )
{
if (buffer_.empty()) return;

uint64_t buf_size = static_cast<uint64_t>(buffer_.size());
uint64_t n = std::min( len, buf_size );

// erase the first n bytes
buffer_.erase( 0, static_cast<size_t>(n) );
bytes_popped_ += n;
}

bool Reader::is_finished() const
{
return closed_ && buffer_.empty();
}

uint64_t Reader::bytes_buffered() const
{
return static_cast<uint64_t>(buffer_.size());
}

uint64_t Reader::bytes_popped() const
{
return bytes_popped_;
}
