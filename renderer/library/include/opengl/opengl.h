#if defined(_WIN32) || defined(_WIN64)
#include <gl/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#elif __APPLE__
#include "TargetConditionals.h"
#if (TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR) || TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <OpenGL/gl3.h>
#endif
#elif defined(__ANDROID__) || defined(ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(__linux__) || defined(__unix__) || defined(__posix__)
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#else
#error platform not supported.
#endif