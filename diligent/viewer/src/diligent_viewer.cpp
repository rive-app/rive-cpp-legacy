#include "artboard.hpp"
#include "file.hpp"
#include "layout.hpp"
#include "animation/linear_animation_instance.hpp"

// #include "diligent_renderer.hpp"
#include "example_diligent_renderer.hpp"

#if PLATFORM_MACOS
#include "DiligentCore/Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <stdio.h>

using namespace Diligent;

RENDER_DEVICE_TYPE g_RenderDeviceType = RENDER_DEVICE_TYPE_GL;

void debugMessageCallback(enum DEBUG_MESSAGE_SEVERITY Severity,
                          const Char* Message,
                          const Char* Function,
                          const Char* File,
                          int Line)
{
}

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw.\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window =
	    glfwCreateWindow(640, 480, "Rive + DiligentGraphics", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "Failed to create window.\n");
		return -1;
	}

	void* windowHandle = nullptr;
#if PLATFORM_MACOS
	glfwMakeContextCurrent(window);
	windowHandle = static_cast<void*>(glfwGetCocoaWindow(window));
#else
	fprintf(stderr, "Unknown platform.\n");
	assert(false);
#endif

	// Load a rive file
	uint8_t* fileBytes = nullptr;
	unsigned int fileBytesLength = 0;

	std::string filename = "assets/polygon_party.riv";
	FILE* fp = fopen(filename.c_str(), "r");
	fseek(fp, 0, SEEK_END);
	fileBytesLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	delete[] fileBytes;
	fileBytes = new uint8_t[fileBytesLength];
	if (fread(fileBytes, 1, fileBytesLength, fp) != fileBytesLength)
	{
		delete[] fileBytes;
		fprintf(stderr, "failed to read all of %s\n", filename.c_str());
		return 1;
	}
	auto reader = rive::BinaryReader(fileBytes, fileBytesLength);
	rive::File* file = nullptr;
	auto result = rive::File::import(reader, &file);
	if (result != rive::ImportResult::success)
	{
		delete[] fileBytes;
		fprintf(stderr, "failed to import file\n");
		return 1;
	}

	rive::File* currentFile = nullptr;
	rive::Artboard* artboard = nullptr;
	rive::LinearAnimationInstance* animationInstance = nullptr;

	rive::ExampleDiligentRenderer renderer;
	Diligent::SetDebugMessageCallback(debugMessageCallback);
	renderer.deviceType(Diligent::RENDER_DEVICE_TYPE_GL);
	Diligent::NativeWindow nw{windowHandle};
	renderer.initialize(&nw);
	renderer.initializePipeline();

	int width = 0, height = 0;
	int lastScreenWidth = 0, lastScreenHeight = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		if (width != lastScreenWidth || height != lastScreenHeight)
		{
			renderer.resize(width, height);
			lastScreenWidth = width;
			lastScreenHeight = height;
		}
		renderer.render();

#if PLATFORM_MACOS
		glfwSwapBuffers(window);
#else
		Present();
#endif

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}