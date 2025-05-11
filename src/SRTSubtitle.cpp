#include "SRTSubtitle.h"
#include <fstream>
#include <regex>
#include <cstdio>      // sscanf / sprintf

/* ──────────────────────────────────────────────────────────────
 *  Вспомогательные функции: <—> миллисекунды
 * ────────────────────────────────────────────────────────────── */

// «HH:MM:SS,mmm»  →  миллисекунды
static long long parseSRTTime(const std::string& timestr)
{
    int h, m, s, ms;
    std::sscanf(timestr.c_str(), "%d:%d:%d,%d", &h, &m, &s, &ms);
    return ((h * 3600 + m * 60 + s) * 1000 + ms);
}

// миллисекунды  →  «HH:MM:SS,mmm»
static std::string formatSRTTime(long long ms)
{
    int h  = ms / 3600000;  ms %= 3600000;
    int m  = ms / 60000;    ms %= 60000;
    int s  = ms / 1000;     ms %= 1000;
    char buf[32];
    std::sprintf(buf, "%02d:%02d:%02d,%03d", h, m, s, static_cast<int>(ms));
    return buf;
}

/* ──────────────────────────────────────────────────────────────
 *  Чтение SubRip (.srt)
 * ────────────────────────────────────────────────────────────── */
void SRTSubtitle::read(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open " + filename);

    cues_.clear();
    std::string line;

    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;                // пустые строки между блоками

        Cue cue;
        cue.index = std::stoi(line);               // строка с номером реплики

        // ----- строка с тайм-кодом «00:00:01,000 --> 00:00:03,000»
        std::getline(ifs, line);
        size_t arrow = line.find("-->");
        cue.start_ms = parseSRTTime(line.substr(0, arrow - 1));
        cue.end_ms   = parseSRTTime(line.substr(arrow + 4));

        // ----- читаем текст (до пустой строки)
        std::string text;
        while (std::getline(ifs, line) && !line.empty())
            text += line + '\n';
        cue.text = text;

        cues_.push_back(cue);
    }
}

/* ──────────────────────────────────────────────────────────────
 *  Запись .srt
 * ────────────────────────────────────────────────────────────── */
void SRTSubtitle::write(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
        throw std::runtime_error("Cannot write " + filename);

    for (const auto& c : cues_)
    {
        ofs << c.index << '\n'
            << formatSRTTime(c.start_ms) << " --> "
            << formatSRTTime(c.end_ms)   << '\n'
            << c.text << '\n';
    }
}

/* ──────────────────────────────────────────────────────────────
 *  stripStyles() — удалить HTML-теги <…>
 * ────────────────────────────────────────────────────────────── */
void SRTSubtitle::stripStyles()
{
    std::regex tag(R"(<[^>]+>)");         // «<…>»
    for (auto& c : cues_)
        c.text = std::regex_replace(c.text, tag, "");
}

/* ──────────────────────────────────────────────────────────────
 *  addDefaultStyle() — добавить единый тег ко всем репликам
 * ────────────────────────────────────────────────────────────── */
void SRTSubtitle::addDefaultStyle(const std::string& style)
{
    // style — это открывающий тег (например, "<b>")
    // автоматически ставим тот же тег как закрывающий
    for (auto& c : cues_)
        c.text = style + c.text + style;
}
