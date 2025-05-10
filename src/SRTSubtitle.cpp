#include "SRTSubtitle.h"
#include <fstream>
#include <regex>
#include <cstdio>

static long long parseSRTTime(const std::string &timestr) {
    int h, m, s, ms;
    std::sscanf(timestr.c_str(), "%d:%d:%d,%d", &h, &m, &s, &ms);
    return ((h*3600 + m*60 + s)*1000 + ms);
}
static std::string formatSRTTime(long long ms) {
    int h = ms/3600000;
    ms %= 3600000;
    int m = ms/60000;
    ms %= 60000;
    int s = ms/1000;
    ms %= 1000;
    char buf[64]; std::sprintf(buf, "%02d:%02d:%02d,%03d", h, m, s, (int)ms);
    return buf;
}

void SRTSubtitle::read(const std::string &filename) {
    std::ifstream ifs(filename);
    cues_.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        Cue cue;
        cue.index = std::stoi(line);
        std::getline(ifs, line);
        auto arrow = line.find("-->");
        cue.start_ms = parseSRTTime(line.substr(0, arrow-1));
        cue.end_ms   = parseSRTTime(line.substr(arrow+4));
        std::string text;
        while (std::getline(ifs, line) && !line.empty()) text += line + "\n";
        cue.text = text;
        cues_.push_back(cue);
    }
}

void SRTSubtitle::write(const std::string &filename) const {
    std::ofstream ofs(filename);
    for (const auto &c : cues_) {
        ofs << c.index << "\n"
            << formatSRTTime(c.start_ms) << " --> " << formatSRTTime(c.end_ms) << "\n"
            << c.text << "\n";
    }
}

void SRTSubtitle::stripStyles() {
    std::regex tag("<[^>]+>");
    for (auto &c : cues_) c.text = std::regex_replace(c.text, tag, "");
}

void SRTSubtitle::addDefaultStyle(const std::string &style) {
    for (auto &c : cues_) c.text = style + c.text + style;
}