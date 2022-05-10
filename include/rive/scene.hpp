#ifndef _RIVE_SCENE_HPP_
#define _RIVE_SCENE_HPP_

#include "rive/animation/loop.hpp"
#include "rive/math/aabb.hpp"
#include "rive/math/vec2d.hpp"
#include <string>

namespace rive {
    class ArtboardInstance;
    class Renderer;

    class SMIInput;
    class SMIBool;
    class SMINumber;
    class SMITrigger;

    class Scene {    
    protected:
        ArtboardInstance* m_ArtboardInstance;

        Scene(ArtboardInstance*);

    public:
        virtual ~Scene() {}
    
        float width() const;
        float height() const;
        AABB bounds() const { return {0, 0, this->width(), this->height()}; }

        virtual std::string name() const = 0;

        // Returns onShot if this has no looping (e.g. a statemachine)
        virtual Loop loop() const = 0;
        // Returns true iff the Scene is known to not be fully opaque
        virtual bool isTranslucent() const = 0;
        // returns -1 for continuous
        virtual float durationSeconds() const = 0;
    
        // returns true if draw() should be called
        virtual bool advanceAndApply(float elapsedSeconds) = 0;

        void draw(Renderer*);

        virtual void pointerDown(Vec2D);
        virtual void pointerMove(Vec2D);
        virtual void pointerUp(Vec2D);

        virtual size_t inputCount() const;
        virtual SMIInput* input(size_t index) const;
        virtual SMIBool* getBool(const std::string&) const;
        virtual SMINumber* getNumber(const std::string&) const;
        virtual SMITrigger* getTrigger(const std::string&) const;
    };

} // namespace rive

#endif
