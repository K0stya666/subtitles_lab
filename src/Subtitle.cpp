#include "Subtitle.h"
#include <algorithm>     // std::max
#include "VTTSubtitle.h" // используем как «универсальный контейнер» для collisions()

/* ──────────────────────────────────────────────────────────────
 *  offset()
 * ──────────────────────────────────────────────────────────────
 *  Сдвигает время всех реплик на delta_ms.
 *    apply_to = 1 → только start
 *    apply_to = 2 → только end
 *    apply_to = 3 → оба (по умолчанию)
 *
 *  • Защита от отрицательного времени: std::max(0, …)
 *  • При сдвиге end-time следим, чтобы он не оказался < start_ms.
 */
void Subtitle::offset(long long delta_ms, int apply_to /* =3 */)
{
    for (auto& cue : cues_)
    {
        if (apply_to & 1)                      // изменить начало?
            cue.start_ms = std::max(0LL, cue.start_ms + delta_ms);

        if (apply_to & 2)                      // изменить конец?
            cue.end_ms = std::max(cue.start_ms, cue.end_ms + delta_ms);
    }
}

/* ──────────────────────────────────────────────────────────────
 *  collisions()
 * ──────────────────────────────────────────────────────────────
 *  Возвращает НОВЫЙ объект (мы берём VTTSubtitle просто как удобный
 *  «контейнер» — формат тут не важен), содержащий только те реплики,
 *  у которых временные интервалы пересекаются друг с другом.
 *
 *  Логика пересечения интервалов [a.start, a.end) и [b.start, b.end):
 *     a.start < b.end   &&   b.start < a.end
 */
Subtitle* Subtitle::collisions() const
{
    auto* result = new VTTSubtitle();          // сырой ptr, владелец — вызывающий

    for (const auto& a : cues_)
    {
        for (const auto& b : cues_)
        {
            if (a.index == b.index) continue;  // не сравниваем сам с собой

            bool overlap =
                    (a.start_ms < b.end_ms) &&
                    (b.start_ms < a.end_ms);

            if (overlap)
            {
                result->cues_.push_back(a);    // добавляем реплику «a» один раз
                break;                         // …и перестаём сравнивать «a» с остальными
            }
        }
    }
    return result;                             // ответственность за delete на вызывающей стороне
}
