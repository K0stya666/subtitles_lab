#pragma once
#include <string>
#include <vector>

// Структура одной реплики
struct Cue {
    int index;           // номер реплики
    long long start_ms;  // время начала в миллисекундах
    long long end_ms;    // время окончания в миллисекундах
    std::string text;    // текст реплики
};

// Базовый класс для субтитров
class Subtitle {
public:
    virtual ~Subtitle() = default;
    virtual void read(const std::string &filename) = 0;
    virtual void write(const std::string &filename) const = 0;

    // Удалить все стили в тексте
    virtual void stripStyles() = 0;
    // Добавить дефолтный стиль (например, обёртка тегом)
    virtual void addDefaultStyle(const std::string &style) = 0;
    // Сдвинуть все реплики на delta_ms (маска apply_to: 1=start,2=end,3=both)
    void offset(long long delta_ms, int apply_to = 3);
    // Вернуть новый объект с перекрывающимися репликами
    Subtitle* collisions() const;

public:
    std::vector<Cue> cues_;
};