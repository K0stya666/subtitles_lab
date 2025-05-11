#pragma once
#include "Subtitle.h"

/*
 * ──────────────────────────────────────────────────────────────────────────────
 *  Класс SAMISubtitle
 * ──────────────────────────────────────────────────────────────────────────────
 *  Представляет субтитры в формате SAMI (.smi) ― «Synchronized Accessible
 *  Media Interchange», разработанный Microsoft.  Внутри тегов <SYNC> указан
 *  атрибут  Start=…  (и опционально End=…), задающий время в миллисекундах
 *  от начала ролика, а текст располагается между <P>…</P>.
 *
 *  Пример:
 *      <SYNC Start=1000 End=3000><P>Hello, world!</P></SYNC>
 *
 *  Мы реализуем базовый набор возможностей:
 *      • чтение / запись <SYNC> с атрибутами Start / End;
 *      • удаление HTML-тегов внутри текста реплик;
 *      • добавление единого «дефолтного» тега/стиля ко всем репликам.
 *
 *  Общие операции (offset, collisions и т. д.) находятся в базовом Subtitle.
 */
class SAMISubtitle : public Subtitle {
public:
    /**
     * Прочитать .smi-файл и заполнить вектор cues_.
     * @param filename путь к входному файлу SAMI.
     * Бросает std::runtime_error, если файл недоступен или неправилен.
     */
    void read(const std::string& filename) override;

    /**
     * Сохранить текущие реплики в файл SAMI.
     * @param filename путь к выходному файлу.
     * Функция формирует базовую HTML-структуру
     *   <SAMI><HEAD>…</HEAD><BODY>…</BODY></SAMI>
     * и выводит каждую реплику отдельным <SYNC>.
     */
    void write(const std::string& filename) const override;

    /**
     * Удалить все HTML/SGML-теги (<…>) из текста реплик.
     * Оставляет «чистый» текст без оформления.
     */
    void stripStyles() override;

    /**
     * Добавить дефолтный стиль ко всем репликам.
     * Например,  addDefaultStyle("<font color=\"red\">")
     * превратит «Text» → «<font color="red">Text</font color="red">».
     *
     * @param style Строка-префикс (без закрывающего слеша), которая будет
     *              добавлена до текста; тот же стиль автоматически ставится
     *              после текста (симметричное закрытие).
     */
    void addDefaultStyle(const std::string& style) override;
};
