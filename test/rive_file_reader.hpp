#ifndef _RIVE_FILE_READER_HPP_
#define _RIVE_FILE_READER_HPP_

#include <rive/file.hpp>
#include "rive_testing.hpp"
#include "no_op_factory.hpp"
#include "../src/render_counter.hpp"

static inline std::vector<uint8_t> ReadFile(const char path[]) {
    FILE* fp = fopen(path, "rb");
    REQUIRE(fp);

    fseek(fp, 0, SEEK_END);
    const size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::vector<uint8_t> bytes(length);
    REQUIRE(fread(bytes.data(), 1, length, fp) == length);
    fclose(fp);

    return bytes;
}

static inline std::unique_ptr<rive::File>
ReadRiveFile(const char path[],
             rive::Factory* factory = nullptr,
             rive::FileAssetResolver* resolver = nullptr) {
    if (!factory) {
        factory = &rive::gNoOpFactory;
    }
    
    auto bytes = ReadFile(path);

    rive::ImportResult result;
    auto file = rive::File::import(rive::toSpan(bytes), factory, &result, resolver);
    REQUIRE(result == rive::ImportResult::success);
    REQUIRE(file.get() != nullptr);
    REQUIRE(file->artboard() != nullptr);

    return file;
}

class RenderObjectLeakChecker {
    rive::RenderCounter m_Before;

public:
    RenderObjectLeakChecker() : m_Before(rive::RenderCounter::globalCounter()) {}
    ~RenderObjectLeakChecker() {
        auto after = rive::RenderCounter::globalCounter();
        for (int i = 0; i <= rive::RenderCounterType::kLastCounterType; ++i) {
            if (after.counts[i] != m_Before.counts[i]) {
                m_Before.dump("before");
                after.dump("after");
                REQUIRE(false);
            }
        }
    }
};

#endif
