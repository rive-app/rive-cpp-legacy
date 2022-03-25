#include "rive/artboard.hpp"
#include "rive/importers/artboard_importer.hpp"
#include "rive/animation/linear_animation.hpp"
#include "rive/animation/state_machine.hpp"
#include "rive/artboard.hpp"

using namespace rive;

ArtboardImporter::ArtboardImporter(Artboard* artboard) : m_Artboard(artboard) {}

void ArtboardImporter::addComponent(Core* object) { m_Artboard->addObject(object); }

void ArtboardImporter::addAnimation(LinearAnimation* animation) {
    m_Artboard->addAnimation(animation);
}

void ArtboardImporter::addStateMachine(StateMachine* stateMachine) {
    m_Artboard->addStateMachine(stateMachine);
}

StatusCode ArtboardImporter::resolve() { return m_Artboard->initialize(); }

bool ArtboardImporter::readNullObject() {
    addComponent(nullptr);
    return true;
}