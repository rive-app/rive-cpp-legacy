
const char* vertexShaderSource = R"""(
#version 330 core

layout (location = 0) in vec2 position;

out vec2 pos;

uniform mat4 projection;
uniform mat4 transform;
uniform mat4 localTransform;

void main() 
{
    gl_Position = projection*transform*vec4(position, 0.0, 1.0);
    pos = (localTransform*vec4(position, 0.0, 1.0)).xy;
}
)""";

const char* fragmentShaderSource = R"""(
#version 330 core

#ifdef GL_ES
precision highp float;
#endif

uniform vec4 color;
uniform vec4 position;
uniform int count;
uniform vec4 colors[16];
uniform float stops[16];
uniform int fillType;
in vec2 pos;
out vec4 fragColor;

void main()
{
    if (fillType == 0)
    {
        // solid
        fragColor = color;//vec4(color.rgb * color.a, color.a);
    }
    else if (fillType == 1)
    {
        // linear

        vec2 start = position.xy;
        vec2 end = position.zw;

        
        vec2 toEnd = end - start;
        float lengthSquared = toEnd.x * toEnd.x + toEnd.y * toEnd.y;
        float f = dot(pos - start, toEnd) / lengthSquared;
        fragColor =
            mix(colors[0], colors[1], smoothstep(stops[0], stops[1], f));
        for (int i = 1; i < 15; ++i)
        {
            if (i >= count - 1)
            {
                break;
            }
            fragColor = mix(fragColor,
                            colors[i + 1],
                            smoothstep(stops[i], stops[i + 1], f));
        }
        // float alpha = fragColor.w;
        // fragColor = vec4(fragColor.xyz * alpha, alpha);
    }
    else if (fillType == 2)
    {
        // radial

        vec2 start = position.xy;
        vec2 end = position.zw;
        
        float f = distance(start, pos) / distance(start, end);
        fragColor =
            mix(colors[0], colors[1], smoothstep(stops[0], stops[1], f));
        for (int i = 1; i < 15; ++i)
        {
            if (i >= count - 1)
            {
                break;
            }
            fragColor = mix(fragColor,
                            colors[i + 1],
                            smoothstep(stops[i], stops[i + 1], f));
        }
        // float alpha = fragColor.w;
        // fragColor = vec4(fragColor.xyz * alpha, alpha);
    }
}
)""";