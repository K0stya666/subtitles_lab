#pragma once
#include <string>
#include <vector>

/* ──────────────────────────────────────────────────────────────
 *  Структура Cue  —  одна реплика субтитров
 * ──────────────────────────────────────────────────────────────
 *  index      — порядковый номер (начинаем с 1, как в SRT);
 *  start_ms   — время начала показа, миллисекунды от 0;
 *  end_ms     — время конца показа, в миллисекундах;
 *  text       — сама строка субтитра (может содержать \n).
 */
struct Cue {
    int         index{0};
    long long   start_ms{0};
    long long   end_ms{0};
    std::string text;
};

/* ──────────────────────────────────────────────────────────────
 *  Абстрактный базовый класс Subtitle
 * ──────────────────────────────────────────────────────────────
 *  Определяет единый интерфейс для разных форматов (.srt, .smi, .ass, .vtt …).
 *  Потомки перегружают read() / write() / stripStyles() / addDefaultStyle().
 *
 *  Общие операции (offset, collisions) реализованы здесь,
 *  т.к. не зависят от конкретного текстового формата.
 */
class Subtitle {
public:
    virtual ~Subtitle() = default;

    /* ────────────── Чтение / запись ────────────── */
    virtual void read (const std::string& filename)          = 0;
    virtual void write(const std::string& filename)   const  = 0;

    /* ────────────── Работа со стилями ───────────── */
    /// Удалить все элементы форматирования (теги, override-теги и т. д.)
    virtual void stripStyles() = 0;

    /// Добавить «дефолтный» стиль ко всем репликам
    /// @param style  строка-префикс (например, "<b>" или "{\\b1}")
    virtual void addDefaultStyle(const std::string& style) = 0;

    /* ────────────── Временные операции ───────────── */
    /// Сдвинуть тайм-коды на delta_ms.
    /// @param delta_ms   величина сдвига (+-), миллисекунды
    /// @param apply_to   1 = только start, 2 = только end, 3 = оба (по умолчанию)
    void offset(long long delta_ms, int apply_to = 3);

    /* ────────────── Поиск перекрытий ───────────── */
    /// Вернуть НОВЫЙ объект (того же типа, что и VTTSubtitle в реализации)
    /// содержащий только реплики, которые пересекаются по времени.
    Subtitle* collisions() const;

    /* ────────────── Данные ───────────── */
public:
    std::vector<Cue> cues_;   ///< вектор всех реплик (виден наследникам)
};
