#pragma once

#include "inplace_function.hpp"
#include <chrono>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cassert>

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

        size_t slot_offset = std::min(ticks, WHEEL_SIZE - 1);
        size_t target_slot = (current_slot_ + slot_offset) % WHEEL_SIZE;

        timeout_id id = next_id_++;
        if (next_id_ == 0) {
            next_id_ = 1;
        }

        slots_[target_slot].entries.push_back({id, std::move(cb), ticks});
        id_to_slot_[id] = target_slot;
        return id;
    }

    bool cancel(timeout_id id) {
        auto slot_it = id_to_slot_.find(id);
        if (slot_it == id_to_slot_.end()) {
            return false;
        }

        size_t slot_idx = slot_it->second;
        auto& entries = slots_[slot_idx].entries;
        auto it = std::find_if(entries.begin(), entries.end(),
            [id](const entry& e) { return e.id == id; });

        if (it != entries.end()) {
            entries.erase(it);
            id_to_slot_.erase(slot_it);
            return true;
        }

        id_to_slot_.erase(slot_it);
        return false;
    }

    void tick() {
        auto& current = slots_[current_slot_];

        for (auto& e : current.entries) {
            id_to_slot_.erase(e.id);

            if (e.remaining_ticks <= 1) {
                e.callback();
            } else {
                e.remaining_ticks--;
                size_t slot_offset = std::min(e.remaining_ticks, WHEEL_SIZE - 1);
                size_t new_slot = (current_slot_ + slot_offset) % WHEEL_SIZE;
                id_to_slot_[e.id] = new_slot;
                slots_[new_slot].entries.push_back(std::move(e));
            }
        }

        current.entries.clear();
        current_slot_ = (current_slot_ + 1) % WHEEL_SIZE;
    }

    size_t pending_count() const {
        size_t count = 0;
        for (const auto& s : slots_) {
            count += s.entries.size();
        }
        return count;
    }

private:
    struct entry {
        timeout_id id;
        callback_fn callback;
        size_t remaining_ticks;
    };

    struct slot {
        std::vector<entry> entries;
    };

    std::vector<slot> slots_;
    std::unordered_map<timeout_id, size_t> id_to_slot_;
    size_t current_slot_;
    timeout_id next_id_;
};

} // namespace katana
