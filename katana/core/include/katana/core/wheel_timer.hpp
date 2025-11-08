#pragma once

#include "inplace_function.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace katana {

template <size_t NumSlots = 512, size_t SlotMs = 100>
class wheel_timer {
public:
    using callback_fn = inplace_function<void(), 128>;
    using timeout_id = uint64_t;

    static constexpr size_t WHEEL_SIZE = NumSlots;
    static constexpr size_t TICK_MS = SlotMs;

    wheel_timer() : current_slot_(0), next_id_(1) {
        slots_.resize(WHEEL_SIZE);
    }

    timeout_id add(std::chrono::milliseconds timeout, callback_fn cb) {
        assert(cb && "Callback must be valid");

        size_t ticks = (static_cast<size_t>(timeout.count()) + TICK_MS - 1) / TICK_MS;
        if (ticks == 0) ticks = 1;

        size_t slot_offset = ticks % WHEEL_SIZE;
        size_t target_slot = (current_slot_ + slot_offset) % WHEEL_SIZE;

        timeout_id id = next_id_++;
        if (next_id_ == 0) {
            next_id_ = 1;
        }

        slots_[target_slot].entry_ids.push_back(id);
        entries_[id] = {std::move(cb), ticks / WHEEL_SIZE, target_slot};
        return id;
    }

    bool cancel(timeout_id id) {
        auto it = entries_.find(id);
        if (it == entries_.end()) {
            return false;
        }

        size_t slot_idx = it->second.slot_idx;
        auto& slot_ids = slots_[slot_idx].entry_ids;

        // Mark as cancelled by removing from entries map
        // The slot's entry_ids will be cleaned during tick()
        entries_.erase(it);

        // Optionally: eagerly remove from slot for better memory usage
        auto slot_it = std::find(slot_ids.begin(), slot_ids.end(), id);
        if (slot_it != slot_ids.end()) {
            slot_ids.erase(slot_it);
        }

        return true;
    }

    void tick() {
        auto& current = slots_[current_slot_];

        for (timeout_id id : current.entry_ids) {
            auto it = entries_.find(id);
            if (it == entries_.end()) {
                continue;
            }

            auto& e = it->second;
            if (e.remaining_ticks == 0) {
                auto cb = std::move(e.callback);
                entries_.erase(it);
                cb();
            } else {
                --e.remaining_ticks;
            }
        }

        current.entry_ids.erase(std::remove_if(current.entry_ids.begin(),
                                               current.entry_ids.end(),
                                               [this](timeout_id id) {
                                                   auto it = entries_.find(id);
                                                   return it == entries_.end();
                                               }),
                                current.entry_ids.end());
        current_slot_ = (current_slot_ + 1) % WHEEL_SIZE;
    }

    size_t pending_count() const {
        return entries_.size();
    }

private:
    struct entry {
        callback_fn callback;
        size_t remaining_ticks;
        size_t slot_idx;
    };

    struct slot {
        std::vector<timeout_id> entry_ids;
    };

    std::vector<slot> slots_;
    std::unordered_map<timeout_id, entry> entries_;
    size_t current_slot_;
    timeout_id next_id_;
};

} // namespace katana
