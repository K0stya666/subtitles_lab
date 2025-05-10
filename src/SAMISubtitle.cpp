#include "SAMISubtitle.h"
#include <fstream>
#include <regex>
#include <sstream>

// В SAMI используются миллисекунды в атрибутах Start/End
static long long parseSAMIAttr(const std::string &val) {
    return std::stoll(val);
}

void SAMISubtitle::read(const std::string &filename) {
    std::ifstream ifs(filename);
    cues_.clear();
    std::string line;
    bool inBody = false;
    while (std::getline(ifs, line)) {
        // Начало тела
        if (line.find("<BODY>") != std::string::npos) { inBody = true; continue; }
        if (!inBody) continue;
        // Поиск SYNC
        auto pos = line.find("<SYNC");
        if (pos == std::string::npos) continue;
        Cue cue;
        cue.index = cues_.size() + 1;
        // Извлечение Start и End
        std::smatch m;
        std::regex re("Start=(\\d+)(?: End=(\\d+))?");
        if (std::regex_search(line, m, re)) {
            cue.start_ms = parseSAMIAttr(m[1]);
            cue.end_ms = m.size()>2 && m[2].matched ? parseSAMIAttr(m[2]) : cue.start_ms;
        }
        // Извлечение текста между <P> и </P>
        std::regex reText(R"(<P[^>]*>(.*?)</P>)", std::regex::icase);
        if (std::regex_search(line, m, reText)) cue.text = m[1];
        cues_.push_back(cue);
    }
}

void SAMISubtitle::write(const std::string &filename) const {
    std::ofstream ofs(filename);
    // Шапка SAMI
    ofs << "<SAMI>\n<HEAD>\n<STYLE TYPE=\"text/css\">\n<!--\nP { margin-left:2pt; margin-right:2pt; }\n-->\n</STYLE>\n</HEAD>\n<BODY>\n";
    for (const auto &c : cues_) {
        ofs << "<SYNC Start=" << c.start_ms << " End=" << c.end_ms << "><P>"
            << c.text << "</P></SYNC>\n";
    }
    ofs << "</BODY>\n</SAMI>\n";
}

void SAMISubtitle::stripStyles() {
    // Удаляем теги <...>
    std::regex tag("<[^>]+>");
    for (auto &c : cues_) c.text = std::regex_replace(c.text, tag, "");
}

void SAMISubtitle::addDefaultStyle(const std::string &style) {
    // Добавляем перед текстом тег стиля
    for (auto &c : cues_) c.text = style + c.text + style;
}
