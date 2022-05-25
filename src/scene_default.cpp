#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "rive/scene.hpp"

using namespace rive;

class SelfContainedScene : public Scene {
    std::unique_ptr<File> m_File;
    std::unique_ptr<ArtboardInstance> m_ABI;
    std::unique_ptr<Scene> m_Scene;

public:
    SelfContainedScene(std::unique_ptr<File> file,
                       std::unique_ptr<ArtboardInstance> abi,
                       std::unique_ptr<Scene> scene)
        : Scene(abi.get())
        , m_File(std::move(file))
        , m_ABI(std::move(abi))
        , m_Scene(std::move(scene)) {}

    ~SelfContainedScene() = default;

    // Forward to our m_Scene

    std::string name() const { return m_Scene->name(); }
    Loop loop() const { return m_Scene->loop(); }
    bool isTranslucent() const { return m_Scene->isTranslucent(); }
    float durationSeconds() const { return m_Scene->durationSeconds(); }

    bool advanceAndApply(float seconds) { return m_Scene->advanceAndApply(seconds); }
    void draw(Renderer* renderer) { return m_Scene->draw(renderer); }

    void pointerDown(Vec2D pos) { return m_Scene->pointerDown(pos); }
    void pointerMove(Vec2D pos) { return m_Scene->pointerMove(pos); }
    void pointerUp(Vec2D pos) { return m_Scene->pointerUp(pos); }

    size_t inputCount() const { return m_Scene->inputCount(); }
    SMIInput* input(size_t index) const { return m_Scene->input(index); }
    SMIBool* getBool(const std::string& name) const { return m_Scene->getBool(name); }
    SMINumber* getNumber(const std::string& name) const { return m_Scene->getNumber(name); }
    SMITrigger* getTrigger(const std::string& name) const { return m_Scene->getTrigger(name); }
};

std::unique_ptr<Scene> Scene::importDefault(Span<uint8_t> span, Factory* factory) {
    auto file = File::import(span, factory);
    if (file) {
        auto abi = file->artboardDefault();
        if (abi) {
            auto scene = abi->defaultScene();
            if (scene) {
                return std::make_unique<SelfContainedScene>(std::move(file),
                                                            std::move(abi),
                                                            std::move(scene));
            }
        }
    }
    return nullptr;
}