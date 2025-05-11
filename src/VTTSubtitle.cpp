#include "VTTSubtitle.h"
#include <fstream>
#include <regex>
#include <cstdio>      // sscanf / sprintf

/* ──────────────────────────────────────────────────────────────
 *  Вспомогательные функции: время WebVTT ↔ миллисекунды
 * ────────────────────────────────────────────────────────────── */

// «HH:MM:SS.mmm»  →  миллисекунды
static long long parseVTTTime(const std::string& timestr)
{
    int h = 0, m = 0, s = 0, ms = 0;
    char dot;                                // парсим точку-разделитель
    std::sscanf(timestr.c_str(), "%d:%d:%d%c%d", &h, &m, &s, &dot, &ms);
    return ((h * 3600 + m * 60 + s) * 1000 + ms);
}

// миллисекунды  →  «HH:MM:SS.mmm»
static std::string formatVTTTime(long long ms)
{
    int h =  ms / 3600000;  ms %= 3600000;
    int m =  ms / 60000;    ms %= 60000;
    int s =  ms / 1000;     ms %= 1000;
    char buf[32];
    std::sprintf(buf, "%02d:%02d:%02d.%03d", h, m, s, static_cast<int>(ms));
    return buf;
}

/* ──────────────────────────────────────────────────────────────
 *  Чтение WebVTT (.vtt)
 * ────────────────────────────────────────────────────────────── */
void VTTSubtitle::read(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open " + filename);

    cues_.clear();
    std::string line;

    std::getline(ifs, line);                 // «WEBVTT» шапка (пропускаем)

    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;          // пустые строки между блоками

        // Ищем строку с тайм-кодом
        size_t arrow = line.find("-->");
        if (arrow == std::string::npos) continue;

        Cue cue;
        cue.index = static_cast<int>(cues_.size()) + 1;
        cue.start_ms = parseVTTTime(line.substr(0, arrow - 1));
        cue.end_ms   = parseVTTTime(line.substr(arrow + 4));

        // Читаем текст (до пустой строки)
        std::string text;
        while (std::getline(ifs, line) && !line.empty())
            text += line + '\n';
        cue.text = text;

        cues_.push_back(cue);
    }
}

/* ──────────────────────────────────────────────────────────────
 *  Запись .vtt
 * ────────────────────────────────────────────────────────────── */
void VTTSubtitle::write(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
        throw std::runtime_error("Cannot write " + filename);

    ofs << "WEBVTT\n\n";

    for (const auto& c : cues_)
    {
        ofs << c.index << '\n';
        ofs << formatVTTTime(c.start_ms) << " --> "
            << formatVTTTime(c.end_ms)   << '\n';
        ofs << c.text << '\n';
    }
}

/* ──────────────────────────────────────────────────────────────
 *  stripStyles()  — удалить все HTML-теги <…>
 * ────────────────────────────────────────────────────────────── */
void VTTSubtitle::stripStyles()
{
    std::regex tag(R"(<[^>]+>)");            // шаблон «<…>»
    for (auto& c : cues_)
        c.text = std::regex_replace(c.text, tag, "");
}

/* ──────────────────────────────────────────────────────────────
 *  addDefaultStyle() — обернуть текст единым тегом
 * ────────────────────────────────────────────────────────────── */
void VTTSubtitle::addDefaultStyle(const std::string& style)
{
    // style — открывающий тег, например "<c.green>"
    // автоматически дублируем его как закрывающий («</c>» нужно?)
    for (auto& c : cues_)
        c.text = style + c.text + style;
}
