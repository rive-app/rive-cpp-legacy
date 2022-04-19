#ifndef _RIVE_PLAYABLE_HPP_
#define _RIVE_PLAYABLE_HPP_

#include "rive/math/aabb.hpp"
#include "rive/span.hpp"

namespace rive {
    class Factory;
    class FileAssetResolver;
    class Renderer;

    class ArtboardInstance;
    class File;
    class LinearAnimationInstance;
    class StateMachineInstance;

    class SMIInput;
    class SMIBool;
    class SMINumber;
    class SMITrigger;

    class Playable {
    private:
        std::unique_ptr<File> m_File;
        std::unique_ptr<ArtboardInstance> m_ABI;
        std::unique_ptr<LinearAnimationInstance> m_LAI;
        std::unique_ptr<StateMachineInstance> m_SMI;
    
        Playable();

    public:
        static std::unique_ptr<Playable> import(Span<const uint8_t>,
                                                Factory*,
                                                FileAssetResolver* = nullptr);

        static std::unique_ptr<Playable>
        animationAt(File*, size_t artboardIndex, size_t animationIndex);
    
        static std::unique_ptr<Playable>
        stateMachineAt(File*, size_t artboardIndex, size_t machineIndex);
    
        ~Playable();

        AABB bounds() const;

        // returns -1 for continuous
        float durationSeconds() const;

        // returns true if draw() should be called
        bool advance(float elapsedSeconds);

        void draw(Renderer*);

        enum class PointerState {
            kDown,      // pointer changed from 'up' to 'down'
            kUp,        // pointer changed from 'down' to 'up'
            kMoved,     // pointer position changed
        };
        void pointerEvent(Vec2D, PointerState);

        size_t inputCount() const;
        SMIInput* inputAt(size_t index) const;
        SMIBool* boolNamed(const std::string&) const;
        SMINumber* numberNamed(const std::string&) const;
        SMITrigger* triggerNamed(const std::string&) const;
    };

} // namespace rive

#endif
