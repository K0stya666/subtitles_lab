#include "SAMISubtitle.h"
#include <fstream>
#include <regex>
#include <sstream>

/* ──────────────────────────────────────────────────────────────
 *  Вспомогательная функция: атрибуты Start / End в SAMI
 * ──────────────────────────────────────────────────────────────
 *  В SAMI время задаётся прямо в миллисекундах:
 *      <SYNC Start=12345 End=23456>
 */
static long long parseSAMIAttr(const std::string& val)
{
    return std::stoll(val);  // std::stoll → строка → long long
}

/* ──────────────────────────────────────────────────────────────
 *  Чтение SAMI (.smi)
 * ────────────────────────────────────────────────────────────── */
void SAMISubtitle::read(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open " + filename);

    cues_.clear();
    std::string line;
    bool inBody = false;          // ждём <BODY>

    while (std::getline(ifs, line))
    {
        /* --- ищем начало тела документа (<BODY>) --------------------- */
        if (line.find("<BODY>") != std::string::npos)
        { inBody = true; continue; }
        if (!inBody) continue;    // до <BODY> — пропускаем

        /* --- ищем теги <SYNC …> -------------------------------------- */
        auto pos = line.find("<SYNC");
        if (pos == std::string::npos) continue;

        Cue cue;
        cue.index = static_cast<int>(cues_.size()) + 1;

        /* --- извлекаем Start / End ----------------------------------- */
        std::smatch m;
        std::regex re(R"(Start=(\d+)(?:\s+End=(\d+))?)");  // «Start=… [End=…]»
        if (std::regex_search(line, m, re))
        {
            cue.start_ms = parseSAMIAttr(m[1]);
            cue.end_ms   = (m.size() > 2 && m[2].matched)
                           ? parseSAMIAttr(m[2])
                           : cue.start_ms;     // если End отсутствует
        }

        /* --- вытягиваем текст между <P>…</P> ------------------------- */
        std::regex reText(R"(<P[^>]*>(.*?)</P>)", std::regex::icase);
        if (std::regex_search(line, m, reText))
            cue.text = m[1];

        cues_.push_back(cue);
    }
}

/* ──────────────────────────────────────────────────────────────
 *  Запись SAMI (.smi)
 * ────────────────────────────────────────────────────────────── */
void SAMISubtitle::write(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
        throw std::runtime_error("Cannot write " + filename);

    // Генерируем простую «шапку»
    ofs << "<SAMI>\n<HEAD>\n"
        << "<STYLE TYPE=\"text/css\">\n<!--\n"
        << "P { margin-left:2pt; margin-right:2pt; }\n"
        << "-->\n</STYLE>\n</HEAD>\n<BODY>\n";

    // Каждый cue → отдельный <SYNC>
    for (const auto& c : cues_)
    {
        ofs << "<SYNC Start=" << c.start_ms
            << " End="       << c.end_ms   << ">"
            << "<P>"         << c.text     << "</P></SYNC>\n";
    }

    ofs << "</BODY>\n</SAMI>\n";
}

/* ──────────────────────────────────────────────────────────────
 *  stripStyles() — удалить все HTML-теги в тексте
 * ────────────────────────────────────────────────────────────── */
void SAMISubtitle::stripStyles()
{
    std::regex tag(R"(<[^>]+>)");   // любой <…>
    for (auto& c : cues_)
        c.text = std::regex_replace(c.text, tag, "");
}

/* ──────────────────────────────────────────────────────────────
 *  addDefaultStyle() — добавить единый тег к каждому cue
 * ──────────────────────────────────────────────────────────────
 *  Если передаёте, например, "<font color=\"red\">", на выходе будет:
 *      <font color="red">Text</font color="red">
 *  (закрывающий тег формируется примитивно: вставляем ту же строку
 *   со слешем после «<», чтобы не усложнять логику в рамках лабы).
 */
void SAMISubtitle::addDefaultStyle(const std::string& style)
{
    // Определяем, как будет выглядеть закрывающий тег
    std::string close = style;
    if (!close.empty() && close[0] == '<')
        close.insert(1, "/");  // "<font>" → "</font>"

    for (auto& c : cues_)
        c.text = style + c.text + close;
}
