#include <memory>
#include <string>
#include <stdexcept>
#include "Subtitle.h"
#include "SRTSubtitle.h"
#include "SAMISubtitle.h"
#include "ASSSubtitle.h"
#include "VTTSubtitle.h"

// Фабрика обработчиков по расширению файла
std::unique_ptr<Subtitle> createHandler(const std::string &ext) {
    if (ext == "srt") return std::make_unique<SRTSubtitle>();
    if (ext == "smi") return std::make_unique<SAMISubtitle>();
    if (ext == "ass" || ext == "ssa") return std::make_unique<ASSSubtitle>();
    if (ext == "vtt") return std::make_unique<VTTSubtitle>();
    throw std::runtime_error("Unknown format: " + ext);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        return 1;
    }
    std::string in_file = argv[1];
    std::string out_file = argv[2];
    auto in_ext = in_file.substr(in_file.find_last_of('.')+1);
    auto out_ext = out_file.substr(out_file.find_last_of('.')+1);
    auto in_subs = createHandler(in_ext);
    in_subs->read(in_file);
    auto out_subs = createHandler(out_ext);
    *out_subs = *in_subs; // копируем реплики
    out_subs->write(out_file);
    return 0;
}