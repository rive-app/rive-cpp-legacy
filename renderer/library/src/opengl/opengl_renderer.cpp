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
}

bool OpenGLRenderer::initialize(void* data)
{
	fprintf(stderr, "init opengl\n");
	assert(m_VertexShader == 0 && m_FragmentShader == 0 && m_Program == 0);

	m_VertexShader =
	    createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (m_VertexShader == 0)
	{
		fprintf(stderr, "init opengl no 1");
		return false;
	}

	m_FragmentShader =
	    createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (m_FragmentShader == 0)
	{
		fprintf(stderr, "init opengl no 2");
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

	// TODO: CLEANUP
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	printf("VAO: %i\n", vao);
	glUseProgram(m_Program);

	m_ProjectionUniformIndex = glGetUniformLocation(m_Program, "projection");
	m_TransformUniformIndex = glGetUniformLocation(m_Program, "transform");
	GLint position = glGetAttribLocation(m_Program, "position");
	fprintf(stderr, "POSITION: %i\n", position);
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
	glPath->stencil(this, transform());
}

void OpenGLRenderer::clipPath(RenderPath* path) {}

void OpenGLRenderer::startFrame()
{
	// glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_Program);
	glEnableVertexAttribArray(0);
	glUniformMatrix4fv(
	    m_ProjectionUniformIndex, 1, GL_FALSE, m_ModelViewProjection);
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
	auto edgeCount = m_Indices.size() / 3;
	auto targetEdgeCount = contourLength - 2;
	if (edgeCount < targetEdgeCount)
	{

		while (edgeCount < targetEdgeCount)
		{
			m_Indices.push_back(0);
			m_Indices.push_back(edgeCount + 1);
			m_Indices.push_back(edgeCount + 2);
			edgeCount++;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		             edgeCount * 3 * sizeof(unsigned short),
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