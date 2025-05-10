#include "VTTSubtitle.h"
#include <fstream>
#include <regex>
#include <cstdio>

static long long parseVTTTime(const std::string &timestr) {
    int h=0,m=0,s=0,ms=0;
    char dot;
    std::sscanf(timestr.c_str(), "%d:%d:%d%c%d", &h,&m,&s,&dot,&ms);
    return ((h*3600 + m*60 + s)*1000 + ms);
}
static std::string formatVTTTime(long long ms) {
    int h = ms/3600000; ms %= 3600000;
    int m = ms/60000; ms %= 60000;
    int s = ms/1000;  ms %= 1000;
    char buf[64]; std::sprintf(buf, "%02d:%02d:%02d.%03d", h,m,s,(int)ms);
    return buf;
}

void VTTSubtitle::read(const std::string &filename) {
    std::ifstream ifs(filename);
    cues_.clear();
    std::string line;
    std::getline(ifs, line); // пропускаем WEBVTT
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        auto arrow = line.find("-->");
        if (arrow==std::string::npos) continue;
        Cue cue; cue.index = cues_.size()+1;
        cue.start_ms = parseVTTTime(line.substr(0, arrow-1));
        cue.end_ms   = parseVTTTime(line.substr(arrow+4));
        std::string text;
        while (std::getline(ifs, line) && !line.empty()) text += line + "\n";
        cue.text = text;
        cues_.push_back(cue);
    }
}

void VTTSubtitle::write(const std::string &filename) const {
    std::ofstream ofs(filename);
    ofs << "WEBVTT\n\n";
    for (const auto &c : cues_) {
        ofs << c.index << "\n";
        ofs << formatVTTTime(c.start_ms) << " --> " << formatVTTTime(c.end_ms) << "\n";
        ofs << c.text << "\n";
    }
}

void VTTSubtitle::stripStyles() {
    std::regex tag("<[^>]+>");
    for (auto &c : cues_) c.text = std::regex_replace(c.text, tag, "");
}

void VTTSubtitle::addDefaultStyle(const std::string &style) {
    for (auto &c : cues_) c.text = style + c.text + style;
}
