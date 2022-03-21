
// Makes ure gl3w is included before glfw3
#include "GL/gl3w.h"

#define SK_GL
#include "GLFW/glfw3.h"

#include "GrBackendSurface.h"
#include "GrDirectContext.h"
#include "SkCanvas.h"
#include "SkColorSpace.h"
#include "SkSurface.h"
#include "SkTypes.h"
#include "gl/GrGLInterface.h"

#include "rive/animation/linear_animation_instance.hpp"
#include "rive/animation/state_machine_instance.hpp"
#include "rive/animation/state_machine_input_instance.hpp"
#include "rive/animation/state_machine_number.hpp"
#include "rive/animation/state_machine_bool.hpp"
#include "rive/animation/state_machine_trigger.hpp"
#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "rive/layout.hpp"
#include "rive/nested_artboard.hpp"
#include "rive/math/aabb.hpp"
#include "skia_renderer.hpp"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <cmath>
#include <stdio.h>

std::string filename;
const char* artboardName = "Bullet Man";
const char* animationName;

rive::File* currentFile = nullptr;
rive::Artboard* artboard = nullptr;
rive::StateMachineInstance* stateMachineInstance = nullptr;
rive::LinearAnimationInstance* animationInstance = nullptr;
uint8_t* fileBytes = nullptr;
unsigned int fileBytesLength = 0;

int animationIndex = 0;
int stateMachineIndex = -1;

void initStateMachine(int index) {
    stateMachineIndex = index;
    animationIndex = -1;
    assert(fileBytes != nullptr);
    auto reader = rive::BinaryReader(fileBytes, fileBytesLength);
    rive::File* file = nullptr;
    auto result = rive::File::import(reader, &file);
    if (result != rive::ImportResult::success) {
        delete[] fileBytes;
        fprintf(stderr, "failed to import file\n");
        return;
    }
    auto sourceArtboard = file->artboard(artboardName);
    if (!sourceArtboard) {
        sourceArtboard = file->artboard();
    }
    // Artboard should always be instance and hence must be deleted.
    delete artboard;
    artboard = sourceArtboard->instance();
    artboard->advance(0.0f);

    delete animationInstance;
    delete stateMachineInstance;
    delete currentFile;
    animationInstance = nullptr;
    stateMachineInstance = nullptr;

    auto stateMachine = index >= 0 && index < artboard->stateMachineCount()
                            ? artboard->stateMachine(index)
                            : nullptr;
    if (stateMachine != nullptr) {
        stateMachineInstance = new rive::StateMachineInstance(stateMachine);
    }

    currentFile = file;
}

static void dump_inputs(rive::StateMachine* machine) {
    std::unique_ptr<rive::StateMachineInstance> inst(new rive::StateMachineInstance(machine));

    printf("state machine: %s needs advance %d\n",
           machine->name().c_str(), inst->needsAdvance());

    for (size_t i = 0; i < inst->inputCount(); ++i) {
        rive::SMIInput* input = inst->input(i);
        auto input_inst = input->input();
        const char* name = input_inst->name().c_str();
        if (input_inst->is<rive::StateMachineNumber>()) {
            printf("   number: %s\n", name);
        }
        else if (input_inst->is<rive::StateMachineBool>()) {
            printf("     bool: %s\n", name);
        }
        else if (input_inst->is<rive::StateMachineTrigger>()) {
            printf("  trigger: %s\n", name);
        }
    }
}

static void dump_machines(rive::Artboard* artboard) {
    for (size_t i = 0; i < artboard->stateMachineCount(); ++i) {
        auto machine = artboard->stateMachine(i);
        dump_inputs(machine);
    }
}

static void fire(rive::StateMachineInstance* smi, const char trigger[]) {
    auto tr = smi->getTrigger(trigger);
    assert(tr);
    tr->fire();
}

void initAnimation(int index) {
    animationIndex = index;
    stateMachineIndex = -1;
    assert(fileBytes != nullptr);
    auto reader = rive::BinaryReader(fileBytes, fileBytesLength);
    rive::File* file = nullptr;
    auto result = rive::File::import(reader, &file);
    if (result != rive::ImportResult::success) {
        delete[] fileBytes;
        fprintf(stderr, "failed to import file\n");
        return;
    }
    auto sourceArtboard = file->artboard(artboardName);
    if (!sourceArtboard) {
        sourceArtboard = file->artboard();
    }
    dump_machines(sourceArtboard);

    // Artboard should always be instance and hence must be deleted.
    delete artboard;
    artboard = sourceArtboard->instance();
    artboard->advance(0.0f);

    delete animationInstance;
    delete stateMachineInstance;
    delete currentFile;
    animationInstance = nullptr;
    stateMachineInstance = nullptr;

    auto animation = index >= 0 && index < artboard->animationCount()
                         ? artboard->animation(index)
                         : nullptr;
    if (animation != nullptr) {
        animationInstance = new rive::LinearAnimationInstance(animation);
    }

    currentFile = file;
    
    initStateMachine(0);
}

void glfwErrorCallback(int error, const char* description) {
    puts(description);
}

void glfwDropCallback(GLFWwindow* window, int count, const char** paths) {
    // Just get the last dropped file for now...
    printf("dropcallback %d %s\n", count, paths[count-1]);
    filename = paths[count - 1];

    FILE* fp = fopen(filename.c_str(), "r");
    fseek(fp, 0, SEEK_END);
    fileBytesLength = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    delete[] fileBytes;
    fileBytes = new uint8_t[fileBytesLength];
    if (fread(fileBytes, 1, fileBytesLength, fp) != fileBytesLength) {
        delete[] fileBytes;
        fprintf(stderr, "failed to read all of %s\n", filename.c_str());
        return;
    }
    initAnimation(0);
}

static bool handle_click(rive::Artboard* artboard, const char name[]) {
    const struct {
        const char* click;
        const char* machine;
        const char* trigger;
    } triggers[] = {
        { "hand-wick",   "State Machine 1", "Light", },
        { "hand-fire",   "State Machine 1", "Cannon", },
        { "hand-helmet", "State Machine 1", "Helmet", },
    };
    
    for (const auto& t : triggers) {
        if (strcmp(name, t.click) == 0) {
            printf("found\n");
            fire(stateMachineInstance, t.trigger);
            return true;
        }
    }
    return false;
}

static void test_hittest(rive::Artboard* artboard, const SkMatrix& ctm) {
    const auto mouse = ImGui::GetMousePos();

    SkMatrix inv;
    (void)ctm.invert(&inv);
    auto pt = inv.mapXY(mouse.x * 2, mouse.y * 2);
    
    const auto bounds = artboard->bounds();
    if (pt.x() < bounds.left() || pt.x() >= bounds.right() ||
        pt.y() < bounds.top() || pt.y() >= bounds.bottom()) {
        return;
    }
    
    rive::HitInfo hinfo;
    hinfo.area = rive::AABB(pt.fX - 1, pt.fY - 1, pt.fX + 2, pt.fY + 2).round();

    static rive::Core* gPrev;

    auto node = artboard->hitTest(&hinfo);
    if (node != gPrev) {
        gPrev = node;
        const char* name = nullptr;
        if (node) {
            assert(node->is<rive::Component>());

            name = node->as<rive::Component>()->name().c_str();
            printf("(%s) mounts [ ", name);
            for (auto na : hinfo.mounts) {
                const char* naname = nullptr;
                if (na) {
                    naname = na->name().c_str();
                }
                printf("(%s) ", naname);
            }
            printf("]\n");
            
            if (hinfo.mounts.size() == 1) {
                handle_click(artboard, hinfo.mounts[0]->name().c_str());
            }
        } else {
            printf("no hit\n");
        }
    }
}

int main(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--file") == 0) {
            filename = argv[++i];
            printf("file %s\n", filename.c_str());
            continue;
        }
        if (strcmp(argv[i], "--artboard") == 0) {
            artboardName = argv[++i];
            printf("artb %s\n", artboardName);
            continue;
        }
        if (strcmp(argv[i], "--animation") == 0) {
            animationName = argv[++i];
            continue;
        }
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw.\n");
        return 1;
    }
    glfwSetErrorCallback(glfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Rive Viewer", NULL, NULL);
    if (window == nullptr) {
        fprintf(stderr, "Failed to make window or GL.\n");
        glfwTerminate();
        return 1;
    }

    glfwSetDropCallback(window, glfwDropCallback);
    glfwMakeContextCurrent(window);
    if (gl3wInit() != 0) {
        fprintf(stderr, "Failed to make initialize gl3w.\n");
        glfwTerminate();
        return 1;
    }
    // Enable VSYNC.
    glfwSwapInterval(1);

    // Setup ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    io.Fonts->AddFontDefault();

    // Setup Skia
    GrContextOptions options;
    sk_sp<GrDirectContext> context = GrDirectContext::MakeGL(nullptr, options);
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkSurface* surface = nullptr;
    SkCanvas* canvas = nullptr;

    // Render loop.
    int width = 0, height = 0;
    int lastScreenWidth = 0, lastScreenHeight = 0;
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        // Update surface.
        if (surface == nullptr || width != lastScreenWidth ||
            height != lastScreenHeight)
        {
            lastScreenWidth = width;
            lastScreenHeight = height;

            SkColorType colorType =
                kRGBA_8888_SkColorType; // GrColorTypeToSkColorType(GrPixelConfigToColorType(kRGBA_8888_GrPixelConfig));
            //
            // if (kRGBA_8888_GrPixelConfig == kSkia8888_GrPixelConfig)
            // {
            // 	colorType = kRGBA_8888_SkColorType;
            // }
            // else
            // {
            // 	colorType = kBGRA_8888_SkColorType;
            // }

            GrBackendRenderTarget backendRenderTarget(width,
                                                      height,
                                                      0, // sample count
                                                      0, // stencil bits
                                                      framebufferInfo);

            delete surface;
            surface = SkSurface::MakeFromBackendRenderTarget(
                          context.get(),
                          backendRenderTarget,
                          kBottomLeft_GrSurfaceOrigin,
                          colorType,
                          nullptr,
                          nullptr)
                          .release();
            if (surface == nullptr) {
                fprintf(stderr, "Failed to create Skia surface\n");
                return 1;
            }
            canvas = surface->getCanvas();
        }

        double time = glfwGetTime();
        float elapsed = (float)(time - lastTime);
        lastTime = time;

        // Clear screen.
        SkPaint paint;
        paint.setColor(SK_ColorDKGRAY);
        canvas->drawPaint(paint);

        if (artboard != nullptr) {
            if (animationInstance != nullptr) {
                animationInstance->advance(elapsed);
                animationInstance->apply(artboard);
            } else if (stateMachineInstance != nullptr) {
                stateMachineInstance->advance(artboard, elapsed);
            }
            artboard->advance(elapsed);

            rive::SkiaRenderer renderer(canvas);
            renderer.save();
            renderer.align(rive::Fit::contain,
                           rive::Alignment::center,
                           rive::AABB(0, 0, width, height),
                           artboard->bounds());
            auto mx = canvas->getTotalMatrix();

            artboard->draw(&renderer);
            renderer.restore();
            
            test_hittest(artboard, mx);
        }

        context->flush();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (artboard != nullptr) {
            ImGui::Begin(filename.c_str(), nullptr);
            if (ImGui::ListBox(
                    "Animations",
                    &animationIndex,
                    [](void* data, int index, const char** name) {
                        const char* animationName =
                            artboard->animation(index)->name().c_str();
                        *name = animationName;
                        return true;
                    },
                    artboard,
                    artboard->animationCount(),
                    4))
            {
                stateMachineIndex = -1;
                initAnimation(animationIndex);
            }
            if (ImGui::ListBox(
                    "State Machines",
                    &stateMachineIndex,
                    [](void* data, int index, const char** name) {
                        const char* machineName =
                            artboard->stateMachine(index)->name().c_str();
                        *name = machineName;
                        return true;
                    },
                    artboard,
                    artboard->stateMachineCount(),
                    4))
            {
                animationIndex = -1;
                initStateMachine(stateMachineIndex);
            }
            if (stateMachineInstance != nullptr) {

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.6666);

                for (int i = 0; i < stateMachineInstance->inputCount(); i++) {
                    auto inputInstance = stateMachineInstance->input(i);

                    if (inputInstance->input()->is<rive::StateMachineNumber>())
                    {
                        // ImGui requires names as id's, use ## to hide the
                        // label but still give it an id.
                        char label[256];
                        snprintf(label, 256, "##%u", i);

                        auto number =
                            static_cast<rive::SMINumber*>(inputInstance);
                        float v = number->value();
                        ImGui::InputFloat(label, &v, 1.0f, 2.0f, "%.3f");
                        number->value(v);
                        ImGui::NextColumn();
                    } else if (inputInstance->input()
                                   ->is<rive::StateMachineTrigger>()) {
                        // ImGui requires names as id's, use ## to hide the
                        // label but still give it an id.
                        char label[256];
                        snprintf(label, 256, "Fire##%u", i);
                        if (ImGui::Button(label)) {
                            auto trigger =
                                static_cast<rive::SMITrigger*>(inputInstance);
                            trigger->fire();
                        }
                        ImGui::NextColumn();
                    } else if (inputInstance->input()
                                   ->is<rive::StateMachineBool>()) {
                        // ImGui requires names as id's, use ## to hide the
                        // label but still give it an id.
                        char label[256];
                        snprintf(label, 256, "##%u", i);
                        auto boolInput =
                            static_cast<rive::SMIBool*>(inputInstance);
                        bool value = boolInput->value();

                        ImGui::Checkbox(label, &value);
                        boolInput->value(value);
                        ImGui::NextColumn();
                    }
                    ImGui::Text("%s", inputInstance->input()->name().c_str());
                    ImGui::NextColumn();
                }

                ImGui::Columns(1);
            }
            ImGui::End();
        } else {
            ImGui::Text("Drop a .riv file to preview.");
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete currentFile;
    delete[] fileBytes;

    // Cleanup Skia.
    delete surface;
    context = nullptr;

    ImGui_ImplGlfw_Shutdown();

    // Cleanup GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
