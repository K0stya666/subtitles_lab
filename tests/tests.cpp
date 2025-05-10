#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "SRTSubtitle.h"
#include "SAMISubtitle.h"
#include "ASSSubtitle.h"
#include "VTTSubtitle.h"

TEST_CASE("SRT read/write and operations") {
    SRTSubtitle srt;
    srt.read("tests/data/example.srt");
    REQUIRE(srt.collisions()->cues_.empty());
    srt.stripStyles();
    srt.addDefaultStyle("<b>");
    srt.offset(1000);
    REQUIRE_NOTHROW(srt.write("tests/output/test.srt"));
}

TEST_CASE("SAMI read/write") {
    SAMISubtitle smi;
    smi.read("tests/data/example.smi");
    smi.stripStyles();
    smi.addDefaultStyle("<font>");
    REQUIRE_NOTHROW(smi.write("tests/output/test.smi"));
}

TEST_CASE("ASS read/write") {
    ASSSubtitle ass;
    ass.read("tests/data/example.ass");
    ass.stripStyles();
    ass.addDefaultStyle("\\b1");
    REQUIRE_NOTHROW(ass.write("tests/output/test.ass"));
}

TEST_CASE("VTT read/write") {
    VTTSubtitle vtt;
    vtt.read("tests/data/example.vtt");
    vtt.stripStyles();
    vtt.addDefaultStyle("<c>");
    REQUIRE_NOTHROW(vtt.write("tests/output/test.vtt"));
}
