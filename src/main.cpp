#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>          // C++17 – создаём каталог для out-файла

#include "Subtitle.h"
#include "SRTSubtitle.h"
#include "SAMISubtitle.h"
#include "ASSSubtitle.h"
#include "VTTSubtitle.h"

/* ──────────────────────────────────────────────────────────────
 *  1. Фабрика по расширению (для ВЫХОДНОГО формата)
 *     – по требованию задания он определяется именно расширением
 * ────────────────────────────────────────────────────────────── */
static std::unique_ptr<Subtitle> createByExt(const std::string& ext)
{
    if (ext == "srt")                  return std::make_unique<SRTSubtitle>();
    if (ext == "smi")                  return std::make_unique<SAMISubtitle>();
    if (ext == "ass" || ext == "ssa")  return std::make_unique<ASSSubtitle>();
    if (ext == "vtt")                  return std::make_unique<VTTSubtitle>();
    throw std::runtime_error("Unknown format (by extension): " + ext);
}

/* ──────────────────────────────────────────────────────────────
 *  2. Авто-детект формата ВХОДНОГО файла по содержимому
 *     (расширение может быть любым).
 * ────────────────────────────────────────────────────────────── */
static std::unique_ptr<Subtitle> detectInputHandler(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open input file: " + filename);

    std::string firstLine;
    std::getline(ifs, firstLine);

    // WebVTT начинается со «WEBVTT»
    if (firstLine.rfind("WEBVTT", 0) == 0)
        return std::make_unique<VTTSubtitle>();

    // SAMI содержит тег <SAMI> или <SYNC …>
    if (firstLine.find("<SAMI>") != std::string::npos ||
        firstLine.find("<SYNC") != std::string::npos)
        return std::make_unique<SAMISubtitle>();

    // ASS / SSA имеет [Script Info] в первых строках
    if (firstLine.rfind("[Script Info]", 0) == 0)
        return std::make_unique<ASSSubtitle>();

    // Если ничего не совпало – по умолчанию пробуем SRT
    return std::make_unique<SRTSubtitle>();
}

/* ──────────────────────────────────────────────────────────────
 *  MAIN
 *      argv[1] – input-file   (расширение произвольное)
 *      argv[2] – output-file  (именно расширение определяет формат)
 * ────────────────────────────────────────────────────────────── */
int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: converter_subs <input> <output.ext>\n";
            return 1;
        }

        const std::string inFile  = argv[1];
        const std::string outFile = argv[2];

        /* ---- 1. Читаем входной файл ----------------------------------- */
        auto inSubs = detectInputHandler(inFile);   // auto-detect
        inSubs->read(inFile);

        /* ---- 2. Готовим обработчик под нужный ВЫХОДНОЙ формат --------- */
        const std::string outExt = outFile.substr(outFile.find_last_of('.') + 1);
        auto outSubs = createByExt(outExt);

        /* ---- 3. Копируем реплики и сохраняем -------------------------- */
        *outSubs = *inSubs;          // shallow-copy вектора cues_
        // Гарантируем, что директория для out-файла существует
        std::filesystem::create_directories(std::filesystem::path(outFile).parent_path());
        outSubs->write(outFile);

        std::cout << "Converted successfully: " << outFile << '\n';
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}
