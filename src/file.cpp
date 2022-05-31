#include "rive/file.hpp"
#include "rive/runtime_header.hpp"
#include "rive/animation/animation.hpp"
#include "rive/core/field_types/core_color_type.hpp"
#include "rive/core/field_types/core_double_type.hpp"
#include "rive/core/field_types/core_string_type.hpp"
#include "rive/core/field_types/core_uint_type.hpp"
#include "rive/generated/core_registry.hpp"
#include "rive/importers/artboard_importer.hpp"
#include "rive/importers/backboard_importer.hpp"
#include "rive/importers/file_asset_importer.hpp"
#include "rive/importers/import_stack.hpp"
#include "rive/importers/keyed_object_importer.hpp"
#include "rive/importers/keyed_property_importer.hpp"
#include "rive/importers/linear_animation_importer.hpp"
#include "rive/importers/state_machine_importer.hpp"
#include "rive/importers/state_machine_listener_importer.hpp"
#include "rive/importers/state_machine_layer_importer.hpp"
#include "rive/importers/layer_state_importer.hpp"
#include "rive/importers/state_transition_importer.hpp"
#include "rive/animation/blend_state_transition.hpp"
#include "rive/animation/any_state.hpp"
#include "rive/animation/entry_state.hpp"
#include "rive/animation/exit_state.hpp"
#include "rive/animation/animation_state.hpp"
#include "rive/animation/blend_state_1d.hpp"
#include "rive/animation/blend_state_direct.hpp"

// Default namespace for Rive Cpp code
using namespace rive;

#if !defined(RIVE_FMT_U64)
#if defined(__ANDROID__)
#if INTPTR_MAX == INT64_MAX
#define RIVE_FMT_U64 "%lu"
#define RIVE_FMT_I64 "%ld"
#else
#define RIVE_FMT_U64 "%llu"
#define RIVE_FMT_I64 "%lld"
#endif
#elif defined(_WIN32)
#define RIVE_FMT_U64 "%lld"
#define RIVE_FMT_I64 "%llu"
#else
#include <inttypes.h>
#define RIVE_FMT_U64 "%" PRIu64
#define RIVE_FMT_I64 "%" PRId64
#endif
#endif

// Import a single Rive runtime object.
// Used by the file importer.
static Core* readRuntimeObject(BinaryReader& reader, const RuntimeHeader& header) {
    auto coreObjectKey = reader.readVarUintAs<int>();
    auto object = CoreRegistry::makeCoreInstance(coreObjectKey);
    while (true) {
        auto propertyKey = reader.readVarUintAs<uint16_t>();
        if (propertyKey == 0) {
            // Terminator. https://media.giphy.com/media/7TtvTUMm9mp20/giphy.gif
            break;
        }

        if (reader.hasError()) {
            delete object;
            return nullptr;
        }
        if (object == nullptr || !object->deserialize(propertyKey, reader)) {
            // We have an unknown object or property, first see if core knows
            // the property type.
            int id = CoreRegistry::propertyFieldId(propertyKey);
            if (id == -1) {
                // No, check if it's in toc.
                id = header.propertyFieldId(propertyKey);
            }

            if (id == -1) {
                // Still couldn't find it, give up.
                fprintf(
                    stderr, "Unknown property key %d, missing from property ToC.\n", propertyKey);
                delete object;
                return nullptr;
            }

            switch (id) {
                case CoreUintType::id:
                    CoreUintType::deserialize(reader);
                    break;
                case CoreStringType::id:
                    CoreStringType::deserialize(reader);
                    break;
                case CoreDoubleType::id:
                    CoreDoubleType::deserialize(reader);
                    break;
                case CoreColorType::id:
                    CoreColorType::deserialize(reader);
                    break;
            }
        }
    }
    if (object == nullptr) {
        // fprintf(stderr,
        //         "File contains an unknown object with coreType " RIVE_FMT_U64
        //         ", which " "this runtime doesn't understand.\n",
        //         coreObjectKey);
        return nullptr;
    }
    return object;
}

File::File(Factory* factory, FileAssetResolver* assetResolver) :
    m_Factory(factory), m_AssetResolver(assetResolver) {
    assert(factory);
}

File::~File() {}

std::unique_ptr<File> File::import(Span<const uint8_t> bytes,
                                   Factory* factory,
                                   ImportResult* result,
                                   FileAssetResolver* assetResolver) {
    BinaryReader reader(bytes);
    RuntimeHeader header;
    if (!RuntimeHeader::read(reader, header)) {
        fprintf(stderr, "Bad header\n");
        if (result) {
            *result = ImportResult::malformed;
        }
        return nullptr;
    }
    if (header.majorVersion() != majorVersion) {
        fprintf(stderr,
                "Unsupported version %u.%u expected %u.%u.\n",
                header.majorVersion(),
                header.minorVersion(),
                majorVersion,
                minorVersion);
        if (result) {
            *result = ImportResult::unsupportedVersion;
        }
        return nullptr;
    }
    auto file = std::unique_ptr<File>(new File(factory, assetResolver));
    auto readResult = file->read(reader, header);
    if (readResult != ImportResult::success) {
        file.reset(nullptr);
    }
    if (result) {
        *result = ImportResult::success;
    }
    return file;
}

ImportResult File::read(BinaryReader& reader, const RuntimeHeader& header) {
    ImportStack importStack;
    while (!reader.reachedEnd()) {
        auto object = readRuntimeObject(reader, header);
        if (object == nullptr) {
            importStack.readNullObject();
            continue;
        }
        if (object->import(importStack) == StatusCode::Ok) {
            switch (object->coreType()) {
                case Backboard::typeKey:
                    m_Backboard.reset(object->as<Backboard>());
                    break;
                case Artboard::typeKey: {
                    Artboard* ab = object->as<Artboard>();
                    ab->m_Factory = m_Factory;
                    m_Artboards.push_back(std::unique_ptr<Artboard>(ab));
                } break;
            }
        } else {
            fprintf(stderr, "Failed to import object of type %d\n", object->coreType());
            delete object;
            continue;
        }
        ImportStackObject* stackObject = nullptr;
        auto stackType = object->coreType();

        switch (stackType) {
            case Backboard::typeKey:
                stackObject = new BackboardImporter(object->as<Backboard>());
                break;
            case Artboard::typeKey:
                stackObject = new ArtboardImporter(object->as<Artboard>());
                break;
            case LinearAnimation::typeKey:
                stackObject = new LinearAnimationImporter(object->as<LinearAnimation>());
                break;
            case KeyedObject::typeKey:
                stackObject = new KeyedObjectImporter(object->as<KeyedObject>());
                break;
            case KeyedProperty::typeKey: {
                auto importer =
                    importStack.latest<LinearAnimationImporter>(LinearAnimation::typeKey);
                if (importer == nullptr) {
                    return ImportResult::malformed;
                }
                stackObject =
                    new KeyedPropertyImporter(importer->animation(), object->as<KeyedProperty>());
                break;
            }
            case StateMachine::typeKey:
                stackObject = new StateMachineImporter(object->as<StateMachine>());
                break;
            case StateMachineLayer::typeKey: {
                auto artboardImporter = importStack.latest<ArtboardImporter>(ArtboardBase::typeKey);
                if (artboardImporter == nullptr) {
                    return ImportResult::malformed;
                }

                stackObject = new StateMachineLayerImporter(object->as<StateMachineLayer>(),
                                                            artboardImporter->artboard());

                break;
            }
            case EntryState::typeKey:
            case ExitState::typeKey:
            case AnyState::typeKey:
            case AnimationState::typeKey:
            case BlendState1D::typeKey:
            case BlendStateDirect::typeKey:
                stackObject = new LayerStateImporter(object->as<LayerState>());
                stackType = LayerState::typeKey;
                break;
            case StateTransition::typeKey:
            case BlendStateTransition::typeKey:
                stackObject = new StateTransitionImporter(object->as<StateTransition>());
                stackType = StateTransition::typeKey;
                break;
            case StateMachineListener::typeKey:
                stackObject = new StateMachineListenerImporter(object->as<StateMachineListener>());
                break;
            case ImageAsset::typeKey:
                stackObject =
                    new FileAssetImporter(object->as<FileAsset>(), m_AssetResolver, m_Factory);
                stackType = FileAsset::typeKey;
                break;
        }
        if (importStack.makeLatest(stackType, stackObject) != StatusCode::Ok) {
            // Some previous stack item didn't resolve.
            return ImportResult::malformed;
        }
    }

    return !reader.hasError() && importStack.resolve() == StatusCode::Ok ? ImportResult::success
                                                                         : ImportResult::malformed;
}

Artboard* File::artboard(std::string name) const {
    for (const auto& artboard : m_Artboards) {
        if (artboard->name() == name) {
            return artboard.get();
        }
    }
    return nullptr;
}

Artboard* File::artboard() const {
    if (m_Artboards.empty()) {
        return nullptr;
    }
    return m_Artboards[0].get();
}

Artboard* File::artboard(size_t index) const {
    if (index >= m_Artboards.size()) {
        return nullptr;
    }
    return m_Artboards[index].get();
}

std::string File::artboardNameAt(size_t index) const {
    auto ab = this->artboard(index);
    return ab ? ab->name() : "";
}

std::unique_ptr<ArtboardInstance> File::artboardDefault() const {
    auto ab = this->artboard();
    return ab ? ab->instance() : nullptr;
}

std::unique_ptr<ArtboardInstance> File::artboardAt(size_t index) const {
    auto ab = this->artboard(index);
    return ab ? ab->instance() : nullptr;
}

std::unique_ptr<ArtboardInstance> File::artboardNamed(std::string name) const {
    auto ab = this->artboard(name);
    return ab ? ab->instance() : nullptr;
}
