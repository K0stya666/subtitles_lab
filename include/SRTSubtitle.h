#pragma once
#include "Subtitle.h"

class SRTSubtitle : public Subtitle {
public:
    void read(const std::string &filename) override;
    void write(const std::string &filename) const override;
    void stripStyles() override;
    void addDefaultStyle(const std::string &style) override;
};