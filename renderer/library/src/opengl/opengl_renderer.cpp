#include "opengl/opengl_renderer.hpp"
#include "opengl/opengl_render_path.hpp"
#include "opengl/opengl_render_paint.hpp"
#include "opengl_shaders.cpp"
#include <cassert>

using namespace rive;

GLuint createAndCompileShader(GLuint type, const char* source);

OpenGLRenderer::OpenGLRenderer() {}
OpenGLRenderer::~OpenGLRenderer()
{
	glDeleteProgram(m_Program);
	glDeleteShader(m_VertexShader);
	glDeleteShader(m_FragmentShader);
	glDeleteBuffers(1, &m_IndexBuffer);
	glDeleteVertexArrays(1, &m_VertexArray);
}

bool OpenGLRenderer::initialize(void* data)
{
	assert(m_VertexShader == 0 && m_FragmentShader == 0 && m_Program == 0);

	m_VertexShader =
	    createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (m_VertexShader == 0)
	{
		return false;
	}

	m_FragmentShader =
	    createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (m_FragmentShader == 0)
	{
		return false;
	}

	m_Program = glCreateProgram();
	glAttachShader(m_Program, m_VertexShader);
	glAttachShader(m_Program, m_FragmentShader);
	glLinkProgram(m_Program);
	GLint isLinked = 0;
	glGetProgramiv(m_Program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_Program, maxLength, &maxLength, &infoLog[0]);
		fprintf(stderr, "Failed to link program %s\n", &infoLog[0]);
		return false;
	}

	// Create index buffer which we'll grow and populate as necessary.
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

	// Two triangles for bounds.
	m_Indices.emplace_back(0);
	m_Indices.emplace_back(1);
	m_Indices.emplace_back(2);
	m_Indices.emplace_back(2);
	m_Indices.emplace_back(3);
	m_Indices.emplace_back(0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	             m_Indices.size() * sizeof(unsigned short),
	             &m_Indices[0],
	             GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);

	glUseProgram(m_Program);

	m_ProjectionUniformIndex = glGetUniformLocation(m_Program, "projection");
	m_TransformUniformIndex = glGetUniformLocation(m_Program, "transform");

	float projection[16] = {0.0f};
	orthographicProjection(projection, 0.0f, 800, 800, 0.0f, 0.0f, 1.0f);
	modelViewProjection(projection);

	return true;
}

void OpenGLRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
	auto glPaint = static_cast<OpenGLRenderPaint*>(paint);
	if (glPaint->style() == RenderPaintStyle::stroke)
	{
		return;
	}

	auto glPath = static_cast<OpenGLRenderPath*>(path);

	// Set up stencil buffer.
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0x0, 0xFF);
	glColorMask(false, false, false, false);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

	glPath->stencil(this, transform());

	glColorMask(true, true, true, true);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

	glPath->cover(this, transform());
}

void OpenGLRenderer::clipPath(RenderPath* path) {}

void OpenGLRenderer::startFrame()
{
	glUseProgram(m_Program);
	glEnableVertexAttribArray(0);
	glUniformMatrix4fv(
	    m_ProjectionUniformIndex, 1, GL_FALSE, m_ModelViewProjection);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRenderer::endFrame() {}

RenderPaint* OpenGLRenderer::makeRenderPaint()
{
	return new OpenGLRenderPaint();
}
RenderPath* OpenGLRenderer::makeRenderPath() { return new OpenGLRenderPath(); }

void OpenGLRenderer::updateIndexBuffer(std::size_t contourLength)
{
	if (contourLength < 2)
	{
		return;
	}
	auto edgeCount = (m_Indices.size() - 6) / 3;
	auto targetEdgeCount = contourLength - 2;
	if (edgeCount < targetEdgeCount)
	{
		while (edgeCount < targetEdgeCount)
		{
			m_Indices.push_back(3);
			m_Indices.push_back(edgeCount + 4);
			m_Indices.push_back(edgeCount + 5);
			edgeCount++;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		             m_Indices.size() * sizeof(unsigned short),
		             &m_Indices[0],
		             GL_STATIC_DRAW);
	}
}

GLuint createAndCompileShader(GLuint type, const char* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
		fprintf(stderr, "Failed to compile shader %s\n", &infoLog[0]);
		glDeleteShader(shader);

		return 0;
	}

	return shader;
}