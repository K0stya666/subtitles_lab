#pragma once
#include "Subtitle.h"

/*
 * ──────────────────────────────────────────────────────────────────────────────
 *  Класс ASSSubtitle
 * ──────────────────────────────────────────────────────────────────────────────
 *  Представляет субтитры в формате Advanced Sub Station Alpha (.ass / .ssa).
 *
 *  Время хранится в миллисекундах, а при чтении/записи конвертируется
 *  в строковый формат “H:MM:SS.cc” (cc – centiseconds, сотые доли секунды),
 *  принятный в секции [Events] → Start / End.
 *
 *  Пример строки диалога:
 *      Dialogue: 0,0:00:01.00,0:00:03.20,Default,,0,0,0,,Text
 *
 *  Мы реализуем только базовый набор возможностей:
 *      • чтение/запись реплик (без Effect, \move и т.п.);
 *      • удаление внутренних override-тегов { ... };
 *      • добавление единого “дефолтного” override-тега ко всем репликам.
 *
 *  Остальная функциональность (offset, collisions, stripStyles для других
 *  форматов…) реализована в базовом классе Subtitle.
 */
class ASSSubtitle : public Subtitle {
public:
    /**
     * Считать файл формата .ass / .ssa и заполнить вектор cues_.
     * @param filename путь к входному файлу.
     *
     * Бросает std::runtime_error в случае ошибки чтения / формата.
     */
    void read(const std::string& filename) override;

    /**
     * Записать текущие реплики в файл формата .ass.
     * @param filename путь к выходному файлу.
     *
     * Генерируется минимальная валидная «шапка»:
     *   [Script Info]  – комментарий о генераторе,
     *   [V4+ Styles]   – один стиль Default,
     *   [Events]       – список Dialogue.
     */
    void write(const std::string& filename) const override;

    /**
     * Удалить все override-теги вида { \b1 \i0 \c&HFFFFFF& … } в тексте реплик.
     * Оставляет чистый текст без форматирования.
     */
    void stripStyles() override;

    /**
     * Добавить ко всем репликам указанный стиль (override-тег ASS).
     * Например:
     *     addDefaultStyle("\\b1")  →  {\\b1}Text
     *
     * @param style строка без фигурных скобок (они будут добавлены автоматически).
     */
    void addDefaultStyle(const std::string& style) override;
};
