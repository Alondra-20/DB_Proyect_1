#include "LFUPolicy.h"

#include <algorithm>

namespace bufman {

void LFUPolicy::init(std::size_t pool_size) {
    slots_.clear();
    buckets_.clear();
    min_freq_ = 0;
}

void LFUPolicy::on_access(std::size_t frame) {
    auto it = slots_.find(frame);

    if (it == slots_.end()) {
        return;
    }

    auto& slot = it->second;

    std::size_t old_count = slot.count;
    std::size_t new_count = old_count + 1;

    auto bucket_it = buckets_.find(old_count);

    if (bucket_it != buckets_.end()) {
        bucket_it->second.erase(slot.pos);

        if (bucket_it->second.empty()) {
            buckets_.erase(bucket_it);

            if (min_freq_ == old_count) {
                min_freq_ = new_count;
            }
        }
    }

    buckets_[new_count].push_front(frame);

    slot.count = new_count;
    slot.pos = buckets_[new_count].begin();
}

void LFUPolicy::on_load(std::size_t frame) {
    auto& slot = slots_[frame];

    slot.count = 1;

    buckets_[1].push_front(frame);
    slot.pos = buckets_[1].begin();

    min_freq_ = 1;
}

void LFUPolicy::on_remove(std::size_t frame) {
    auto it = slots_.find(frame);

    if (it == slots_.end()) {
        return;
    }

    std::size_t count = it->second.count;

    auto bucket_it = buckets_.find(count);

    if (bucket_it != buckets_.end()) {
        bucket_it->second.erase(it->second.pos);

        if (bucket_it->second.empty()) {
            buckets_.erase(bucket_it);
        }
    }

    slots_.erase(it);

if (buckets_.empty()) {
    min_freq_ = 0;
} else {
    min_freq_ = static_cast<std::size_t>(-1);

    for (const auto& entry : buckets_) {
        if (entry.first < min_freq_) {
            min_freq_ = entry.first;
        }
    }

}

}
std::optional<std::size_t>LFUPolicy::pick_victim(
    const std::vector<std::size_t>& candidates) const {

    std::optional<std::size_t> victim;
    std::size_t lowest_frequency = 0;

    for (std::size_t frame : candidates) {
        auto slot_it = slots_.find(frame);

        if (slot_it == slots_.end()) {
            continue;
        }

        std::size_t frequency = slot_it->second.count;

        if (!victim.has_value() || frequency < lowest_frequency) {
            victim = frame;
            lowest_frequency = frequency;
        }
    }

    if (!victim.has_value()) {
        return std::nullopt;
    }

    auto bucket_it = buckets_.find(lowest_frequency);

    if (bucket_it == buckets_.end()) {
        return victim;
    }

    const auto& bucket = bucket_it->second;

    for (auto it = bucket.rbegin(); it != bucket.rend(); ++it) {
        if (std::find(candidates.begin(),
                      candidates.end(),
                      *it) != candidates.end()) {
            return *it;
        }
    }

    return victim;
}

}  