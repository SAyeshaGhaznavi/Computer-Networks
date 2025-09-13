#include "reassembler.hh"
#include <algorithm>

void Reassembler::insert(uint64_t first_index, string data, bool is_last_substring)
{
    if (data.empty() && !is_last_substring) return;

    if (is_last_substring) eof_index_ = first_index + data.size();

    //waste bytes already written
    if (first_index + data.size() <= next_index_) return;

    if (first_index < next_index_)
    {
        data = data.substr(next_index_ - first_index);
        first_index = next_index_;
    }

    // merge and remove overlapping fragments
    auto it = fragments_.lower_bound(first_index);
    while (it != fragments_.end() && it->first < first_index + data.size())
    {
        size_t overlap = first_index + data.size() - it->first;
        if (overlap < it->second.size())
        {
            it->second = it->second.substr(overlap);
            break;
        } else
        {
            it = fragments_.erase(it);
        }
    }

    fragments_[first_index] = std::move(data);

    //update pending ones
    pending_bytes_ = 0;
    for (auto& [k, v] : fragments_) pending_bytes_ += v.size();

    if (data.empty() && is_last_substring) 
    {
    eof_index_ = first_index;
    assemble_contiguous();  //will close the stream
    return;
}
}

void Reassembler::assemble_contiguous() {
    while (true) {
        auto it = fragments_.find(next_index_);
        if (it == fragments_.end()) break;

        string& chunk = it->second;
        if (chunk.empty()) {
            fragments_.erase(it);
            continue;
        }

        uint64_t writable = min<uint64_t>(chunk.size(), output_.writer().available_capacity());
        if (writable == 0) break;

        output_.writer().push(chunk.substr(0, writable));
        next_index_ += writable;

        if (writable == chunk.size()) {
            fragments_.erase(it);
        } else {
            chunk = chunk.substr(writable);
            break;
        }
    }

    // close if EOF reached and nothing is pending
    if (eof_index_.has_value() && next_index_ >= eof_index_.value()) {
        output_.writer().close();
    }

    // update pending bytes
    pending_bytes_ = 0;
    for (auto& [k,v] : fragments_) pending_bytes_ += v.size();
}


uint64_t Reassembler::count_bytes_pending() const 
{
    return pending_bytes_;
}
