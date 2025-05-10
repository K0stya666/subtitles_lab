#pragma once
#include "Subtitle.h"

// Класс для формата SAMI (.smi)
class SAMISubtitle : public Subtitle {
public:
    void read(const std::string &filename) override;
    void write(const std::string &filename) const override;
    void stripStyles() override;
    void addDefaultStyle(const std::string &style) override;
};