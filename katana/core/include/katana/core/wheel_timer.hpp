#pragma once

#include "inplace_function.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace katana {

template <size_t NumSlots = 512, size_t SlotMs = 100> class wheel_timer {
public:
    using callback_fn = inplace_function<void(), 128>;
    using timeout_id = uint64_t;
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::milliseconds;

    static constexpr size_t WHEEL_SIZE = NumSlots;
    static constexpr size_t TICK_MS = SlotMs;

    wheel_timer() : current_slot_(0), last_tick_(clock::now()) {
        slots_.resize(WHEEL_SIZE);
        entries_.reserve(WHEEL_SIZE);
    }

    timeout_id add(duration timeout, callback_fn cb) {
        // Validate callback - use exception instead of assert for release builds
        if (!cb) {
            throw std::invalid_argument("wheel_timer::add: callback must be valid");
        }

        if (timeout.count() <= 0) {
            timeout = duration{1};
        }

        size_t ticks = (static_cast<size_t>(timeout.count()) + TICK_MS - 1) / TICK_MS;
        if (ticks == 0) {
            ticks = 1;
        }

        size_t slot_offset = ticks % WHEEL_SIZE;
        size_t target_slot = (current_slot_ + slot_offset) % WHEEL_SIZE;
        size_t rounds = ticks / WHEEL_SIZE;

        uint32_t index = acquire_entry();
        auto& entry = entries_[index];
        entry.callback = std::move(cb);
        entry.remaining_rounds = rounds;
        entry.slot_idx = static_cast<uint32_t>(target_slot);
        entry.active = true;

        slot_handle handle{index, entry.generation};
        slots_[target_slot].handles.push_back(handle);

        return make_id(handle);
    }

    [[nodiscard]] bool cancel(timeout_id id) {
        auto [index, generation] = decode_id(id);
        if (index >= entries_.size()) {
            return false;
        }

        auto& entry = entries_[index];
        if (!entry.active || entry.generation != generation) {
            return false;
        }

        auto slot_idx = entry.slot_idx;
        release_entry(index);

        auto& handles = slots_[slot_idx].handles;
        handles.erase(std::remove_if(handles.begin(),
                                     handles.end(),
                                     [&](const slot_handle& h) {
                                         return h.index == index && h.generation == generation;
                                     }),
                      handles.end());

        if (handles.capacity() > handles.size() * 4 && handles.capacity() > 64) {
            handles.shrink_to_fit();
        }

        return true;
    }

    void tick(clock::time_point now = clock::now()) {
        if (now <= last_tick_) {
            return;
        }

        auto elapsed = std::chrono::duration_cast<duration>(now - last_tick_);
        if (elapsed.count() < static_cast<int64_t>(TICK_MS)) {
            return;
        }

        size_t ticks = static_cast<size_t>(elapsed.count()) / TICK_MS;
        last_tick_ += duration(static_cast<int64_t>(ticks) * static_cast<int64_t>(TICK_MS));

        for (size_t i = 0; i < ticks; ++i) {
            advance_slot();
        }
    }

    size_t pending_count() const { return pending_entries_; }

    duration time_until_next_expiration(clock::time_point now = clock::now()) const {
        if (pending_entries_ == 0) {
            return duration::max();
        }

        auto since_last_tick =
            now > last_tick_ ? std::chrono::duration_cast<duration>(now - last_tick_) : duration{0};
        auto base = duration(TICK_MS) - std::min(duration(TICK_MS), since_last_tick);

        duration best = duration::max();
        for (size_t slot = 0; slot < slots_.size(); ++slot) {
            if (slots_[slot].handles.empty()) {
                continue;
            }

            auto offset = slot >= current_slot_ ? slot - current_slot_
                                                : (WHEEL_SIZE - (current_slot_ - slot));

            for (const auto& handle : slots_[slot].handles) {
                if (handle.index >= entries_.size()) {
                    continue;
                }
                const auto& entry = entries_[handle.index];
                if (!entry.active || entry.generation != handle.generation) {
                    continue;
                }

                size_t total_ticks = offset + entry.remaining_rounds * WHEEL_SIZE;
                duration candidate = base + duration(static_cast<int64_t>(total_ticks) *
                                                     static_cast<int64_t>(TICK_MS));
                if (candidate < best) {
                    best = candidate;
                }
            }
        }

        return best;
    }

private:
    struct slot_handle {
        uint32_t index;
        uint32_t generation;
    };

    struct slot_bucket {
        std::vector<slot_handle> handles;
    };

    struct entry_data {
        callback_fn callback;
        size_t remaining_rounds{0};
        uint32_t slot_idx{0};
        uint32_t generation{1};
        bool active{false};
    };

    static timeout_id make_id(slot_handle handle) {
        return (static_cast<timeout_id>(handle.generation) << 32) | handle.index;
    }

    static std::pair<uint32_t, uint32_t> decode_id(timeout_id id) {
        uint32_t index = static_cast<uint32_t>(id & 0xffffffffu);
        uint32_t generation = static_cast<uint32_t>(id >> 32);
        return {index, generation};
    }

    uint32_t acquire_entry() {
        uint32_t index;
        if (!free_list_.empty()) {
            index = free_list_.back();
            free_list_.pop_back();
            auto& entry = entries_[index];
            ++entry.generation;
            if (entry.generation == 0) {
                ++entry.generation;
            }
        } else {
            index = static_cast<uint32_t>(entries_.size());
            entries_.push_back(entry_data{});
        }
        ++pending_entries_;
        return index;
    }

    void release_entry(uint32_t index) {
        auto& entry = entries_[index];
        entry.active = false;
        entry.callback = callback_fn{};
        entry.remaining_rounds = 0;
        entry.slot_idx = 0;
        free_list_.push_back(index);
        if (pending_entries_ > 0) {
            --pending_entries_;
        }
    }

    void advance_slot() {
        current_slot_ = (current_slot_ + 1) % WHEEL_SIZE;
        auto& bucket = slots_[current_slot_];

        if ((current_slot_ + 1) < WHEEL_SIZE) {
            __builtin_prefetch(&slots_[current_slot_ + 1], 0, 3);
        }

        if (bucket.handles.empty()) {
            return;
        }

        auto handles = std::move(bucket.handles);
        bucket.handles.clear();
        bucket.handles.reserve(handles.size());

        for (size_t i = 0; i < handles.size(); ++i) {
            auto& handle = handles[i];

            if (i + 1 < handles.size() && handles[i + 1].index < entries_.size()) {
                __builtin_prefetch(&entries_[handles[i + 1].index], 0, 3);
            }

            if (handle.index >= entries_.size()) {
                continue;
            }
            auto& entry = entries_[handle.index];
            if (!entry.active || entry.generation != handle.generation) {
                continue;
            }

            if (entry.remaining_rounds > 0) {
                --entry.remaining_rounds;
                bucket.handles.push_back(handle);
                continue;
            }

            auto cb = std::move(entry.callback);
            release_entry(handle.index);
            cb();
        }
    }

    std::vector<slot_bucket> slots_;
    std::vector<entry_data> entries_;
    std::vector<uint32_t> free_list_;
    size_t current_slot_;
    clock::time_point last_tick_;
    size_t pending_entries_{0};
};

} // namespace katana
