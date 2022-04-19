#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "rive/file_asset_resolver.hpp"
#include "rive/playable.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "rive/animation/state_machine_instance.hpp"

using namespace rive;

Playable::Playable() {}

Playable::~Playable() {}

std::unique_ptr<Playable> Playable::animationAt(File* file,
                                                size_t artboardIndex,
                                                size_t animationIndex) {
    auto abi = file->artboardAt(artboardIndex);
    if (abi) {
        auto lai = abi->animationAt(animationIndex);
        if (lai) {
            std::unique_ptr<Playable> play(new Playable);
            play->m_ABI = std::move(abi);
            play->m_LAI = std::move(lai);
            return play;
        }
    }
    return nullptr;
}

std::unique_ptr<Playable> Playable::stateMachineAt(File* file,
                                                   size_t artboardIndex,
                                                   size_t machineIndex) {
    auto abi = file->artboardAt(artboardIndex);
    if (abi) {
        auto smi = abi->stateMachineAt(machineIndex);
        if (smi) {
            std::unique_ptr<Playable> play(new Playable);
            play->m_ABI = std::move(abi);
            play->m_SMI = std::move(smi);
            return play;
        }
    }
    return nullptr;
}

std::unique_ptr<Playable> Playable::import(Span<const uint8_t> bytes,
                                           Factory* factory,
                                           FileAssetResolver* resolver)
{
    auto file = File::import(bytes, factory, nullptr, resolver);
    if (!file) {
        return nullptr;
    }

    auto play = Playable::stateMachineAt(file.get(), 0, 0);
    if (!play) {
        play = Playable::animationAt(file.get(), 0, 0);
    }
    if (play) {
        play->m_File = std::move(file);
    }
    return play;
}

AABB Playable::bounds() const {
    return m_ABI->bounds();
}

float Playable::durationSeconds() const {
    return m_SMI ? -1 : m_LAI->durationSeconds();
}

// returns true if draw() should be called
bool Playable::advance(float elapsedSeconds) {
    bool needsRedraw;
    if (m_SMI) {
        needsRedraw = m_SMI->advance(elapsedSeconds);
    } else {
        needsRedraw = m_LAI->advance(elapsedSeconds);
        m_LAI->apply();
    }
    return needsRedraw;
}

void Playable::draw(Renderer* renderer) {
    m_ABI->draw(renderer);
}

void Playable::pointerEvent(Vec2D pos, PointerState state) {
    if (m_SMI) {
        switch (state) {
            case PointerState::kDown:
                m_SMI->pointerDown(pos);
                break;
            case PointerState::kMoved:
                m_SMI->pointerMove(pos);
                break;
            case PointerState::kUp:
                m_SMI->pointerUp(pos);
                break;
        }
    }
    // else the animation just ignores the pointer events
}

// StateMachine Inputs

size_t Playable::inputCount() const {
    return m_SMI ? m_SMI->inputCount() : 0;
}

SMIInput* Playable::inputAt(size_t index) const {
    return m_SMI ? m_SMI->input(index) : nullptr;
}

SMIBool* Playable::boolNamed(const std::string& name) const {
    return m_SMI ? m_SMI->getBool(name) : nullptr;
}

SMINumber* Playable::numberNamed(const std::string& name) const {
    return m_SMI ? m_SMI->getNumber(name) : nullptr;
}

SMITrigger* Playable::triggerNamed(const std::string& name) const {
    return m_SMI ? m_SMI->getTrigger(name) : nullptr;
}
