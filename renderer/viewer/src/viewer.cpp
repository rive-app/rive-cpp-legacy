#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "rive/layout.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "low_level/low_level_renderer.hpp"
#include <string.h>

#if __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "graphics_api.hpp"

// Make sure gl3w is included before glfw3
// #include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <stdio.h>

void* viewerNativeWindowHandle = nullptr;

int main(int argc, const char** argv)
{
	auto graphicsApi = rive::GraphicsApi::unknown;
	if (argc >= 2)
	{
		// Figure out if the user requested a specific Graphics API.
		const char* deviceName = argv[1];
		if (strcmp(deviceName, "gl") == 0 || strcmp(deviceName, "opengl") == 0)
		{
			graphicsApi = rive::GraphicsApi::opengl;
		}
		else if (strcmp(deviceName, "mtl") == 0 ||
		         strcmp(deviceName, "metal") == 0)
		{
			graphicsApi = rive::GraphicsApi::metal;
		}
		else if (strcmp(deviceName, "vk") == 0 ||
		         strcmp(deviceName, "vulkan") == 0)
		{
			graphicsApi = rive::GraphicsApi::vulkan;
		}
		else if (strcmp(deviceName, "d3d11") == 0)
		{
			graphicsApi = rive::GraphicsApi::d3d11;
		}
		else if (strcmp(deviceName, "d3d12") == 0)
		{
			graphicsApi = rive::GraphicsApi::d3d12;
		}
	}

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw.\n");
		return 1;
	}

	if (graphicsApi == rive::GraphicsApi::opengl)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 16);
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Rive", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "Failed to create window.\n");
		return -1;
	}

#ifdef __APPLE__
	glfwMakeContextCurrent(window);
	viewerNativeWindowHandle = static_cast<void*>(glfwGetCocoaWindow(window));
#endif

	// If no specific API requested, big the default one for the platform.
	rive::LowLevelRenderer* renderer =
	    graphicsApi == rive::GraphicsApi::unknown
	        ? rive::GraphicsApi::makeRenderer()
	        : rive::GraphicsApi::makeRenderer(graphicsApi);

	if (renderer == nullptr || !renderer->initialize())
	{
		return 1;
	}

	char windowTitle[128];
	snprintf(windowTitle,
	         128,
	         "Rive Low Level Renderer (%s)",
	         rive::GraphicsApi::name(renderer->type()));
	glfwSetWindowTitle(window, windowTitle);

	// Load a rive file
	uint8_t* fileBytes = nullptr;
	unsigned int fileBytesLength = 0;

	// std::string filename = "assets/polygon_party.riv";
	// std::string filename = "assets/triangle.riv";
	// std::string filename = "assets/juice.riv";
	// std::string filename = "assets/clip.riv";
	// std::string filename = "assets/clipped_circle_star_2.riv";
	// std::string filename = "assets/marty.riv";
	std::string filename = "assets/runner.riv";
	// std::string filename = "assets/rotate_square.riv";
	// std::string filename = "assets/bone_deform.riv";
	// std::string filename = "assets/off_road_car.riv";
	// std::string filename = "assets/simple_stroke.riv";
	// std::string filename = "assets/leg_issues.riv";
	// std::string filename = "assets/control.riv";
	// std::string filename = "assets/stroke_caps.riv";
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
	rive::LinearAnimation* animation = // nullptr;
	    animationIndex >= 0 && animationIndex < artboard->animationCount()
	        ? artboard->animation(animationIndex)
	        : nullptr;
	if (animation != nullptr)
	{
		animationInstance = new rive::LinearAnimationInstance(animation);
	}

	double lastTime = glfwGetTime();
	int lastWidth = 0, lastHeight = 0;
	float projection[16] = {0.0f};
	while (!glfwWindowShouldClose(window))
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);

		if (lastWidth != width || lastHeight != height)
		{
			lastWidth = width;
			lastHeight = height;
			renderer->orthographicProjection(
			    projection, 0.0f, width, height, 0.0f, 0.0f, 1.0f);
			renderer->modelViewProjection(projection);
		}

		double time = glfwGetTime();
		float elapsed = (float)(time - lastTime);
		lastTime = time;

		renderer->startFrame();

		if (artboard != nullptr)
		{
			if (animationInstance != nullptr)
			{
				animationInstance->advance(elapsed * 0.25f);
				animationInstance->apply(artboard);
			}
			artboard->advance(elapsed);
			renderer->save();
			renderer->align(rive::Fit::contain,
			                rive::Alignment::center,
			                rive::AABB(0, 0, width, height),
			                artboard->bounds());
			artboard->draw(renderer);
			renderer->restore();
		}

		renderer->endFrame();

#if __APPLE__
		glfwSwapBuffers(window);
#endif
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}