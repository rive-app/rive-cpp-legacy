#include "artboard.hpp"
#include "file.hpp"
#include "layout.hpp"
#include "animation/linear_animation_instance.hpp"
#include <string.h>

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
	printf("MESSAGE: %s %s %s\n", Message, Function, File);
}

int main(int argc, const char** argv)
{
	Diligent::RENDER_DEVICE_TYPE desiredDeviceType =
	    Diligent::RENDER_DEVICE_TYPE_UNDEFINED;

	if (argc >= 2)
	{
		const char* deviceName = argv[1];
		if (strcmp(deviceName, "mtl") == 0 || strcmp(deviceName, "metal") == 0)
		{
#if METAL_SUPPORTED
			desiredDeviceType = Diligent::RENDER_DEVICE_TYPE_METAL;
#else
			fprintf(stderr, "Metal is not supported.\n");
			return 1;
#endif
		}
		else if (strcmp(deviceName, "vk") == 0 ||
		         strcmp(deviceName, "vulkan") == 0)
		{
#if VULKAN_SUPPORTED
			desiredDeviceType = Diligent::RENDER_DEVICE_TYPE_VULKAN;
#else
			fprintf(stderr, "Vulkan is not supported.\n");
			return 1;
#endif
		}
		else if (strcmp(deviceName, "d3d11") == 0)
		{
#if D3D11_SUPPORTED
			desiredDeviceType = Diligent::RENDER_DEVICE_TYPE_D3D11;
#else
			fprintf(stderr, "Direct3D11 is not supported.\n");
			return 1;
#endif
		}
		else if (strcmp(deviceName, "d3d12") == 0)
		{
#if D3D12_SUPPORTED
			desiredDeviceType = Diligent::RENDER_DEVICE_TYPE_D3D12;
#else
			fprintf(stderr, "Direct3D12 is not supported.\n");
			return 1;
#endif
		}
	}

	if (desiredDeviceType == RENDER_DEVICE_TYPE_UNDEFINED)
	{
#if METAL_SUPPORTED
		desiredDeviceType = RENDER_DEVICE_TYPE_METAL;
#elif VULKAN_SUPPORTED
		desiredDeviceType = RENDER_DEVICE_TYPE_VULKAN;
#elif D3D12_SUPPORTED
		desiredDeviceType = RENDER_DEVICE_TYPE_D3D12;
#elif D3D11_SUPPORTED
		desiredDeviceType = RENDER_DEVICE_TYPE_D3D11;
#elif GL_SUPPORTED
		desiredDeviceType = RENDER_DEVICE_TYPE_GL;
#endif
	}

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw.\n");
		return 1;
	}

	if (desiredDeviceType == RENDER_DEVICE_TYPE_GL)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

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

	rive::Artboard* artboard = file->artboard();
	artboard->advance(0.0f);

	rive::LinearAnimationInstance* animationInstance = nullptr;
	int animationIndex = 0;
	auto animation =
	    animationIndex >= 0 && animationIndex < artboard->animationCount()
	        ? artboard->animation(animationIndex)
	        : nullptr;
	if (animation != nullptr)
	{
		animationInstance = new rive::LinearAnimationInstance(animation);
	}

	rive::ExampleDiligentRenderer renderer;
	Diligent::SetDebugMessageCallback(debugMessageCallback);
	renderer.deviceType(desiredDeviceType);
	Diligent::NativeWindow nw{windowHandle};
	renderer.initialize(&nw);
	renderer.initializePipeline();

	int width = 0, height = 0;
	int lastScreenWidth = 0, lastScreenHeight = 0;
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		if (width != lastScreenWidth || height != lastScreenHeight)
		{
			renderer.resize(width, height);
			lastScreenWidth = width;
			lastScreenHeight = height;
		}

		double time = glfwGetTime();
		float elapsed = (float)(time - lastTime);
		lastTime = time;

		// if (artboard != nullptr)
		// {
		// 	if (animationInstance != nullptr)
		// 	{
		// 		animationInstance->advance(elapsed);
		// 		animationInstance->apply(artboard);
		// 	}
		// 	artboard->advance(elapsed);
		// 	renderer.save();
		// 	renderer.align(rive::Fit::contain,
		// 	               rive::Alignment::center,
		// 	               rive::AABB(0, 0, width, height),
		// 	               artboard->bounds());
		// 	artboard->draw(&renderer);
		// 	renderer.restore();
		// }
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