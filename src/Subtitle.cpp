#include "Subtitle.h"
#include <algorithm>
// Для collisions используем VTTSubtitle как контейнер
#include "VTTSubtitle.h"

void Subtitle::offset(long long delta_ms, int apply_to) {
    for (auto &cue : cues_) {
        if (apply_to & 1) cue.start_ms = std::max(0LL, cue.start_ms + delta_ms);
        if (apply_to & 2) cue.end_ms   = std::max(cue.start_ms, cue.end_ms + delta_ms);
    }
}

Subtitle* Subtitle::collisions() const {
    auto *result = new VTTSubtitle();
    for (const auto &a : cues_) {
        for (const auto &b : cues_) {
            if (a.index != b.index && a.start_ms < b.end_ms && b.start_ms < a.end_ms) {
                result->cues_.push_back(a);
                break;
            }
        }
    }
    return result;
}