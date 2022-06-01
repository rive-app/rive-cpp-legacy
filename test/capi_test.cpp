#include "capi/rive_api.h"

#include "no_op_factory.hpp"
#include <catch.hpp>
#include <cstdio>

static rive::NoOpFactory gNoOpFactory;

static rive_file_t* loadfile(const char path[]) {
    FILE* fp = fopen(path, "rb");
    REQUIRE(fp != nullptr);

    fseek(fp, 0, SEEK_END);
    const size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::vector<uint8_t> bytes(length);
    REQUIRE(fread(bytes.data(), 1, length, fp) == length);
    fclose(fp);

    rive_span_t span = {bytes.data(), length};
    return rive_file_import(span, (rive_factory_t*)&gNoOpFactory);
}

TEST_CASE("capi", "[file]") {
    auto file = loadfile("../../test/assets/two_artboards.riv");
    
    REQUIRE(rive_file_artboard_count(file) == 2);

    rive_file_delete(file);
}
