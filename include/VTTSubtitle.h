#pragma once
#include "Subtitle.h"

// Класс для формата WebVTT
class VTTSubtitle : public Subtitle {
public:
    // Чтение .vtt файла
    void read(const std::string &filename) override;
    // Запись .vtt файла
    void write(const std::string &filename) const override;
    // Удаление всех стилей (<c>, <v> и др.)
    void stripStyles() override;
    // Добавление дефолтного стиля (например, обёртка в <c>)
    void addDefaultStyle(const std::string &style) override;
};