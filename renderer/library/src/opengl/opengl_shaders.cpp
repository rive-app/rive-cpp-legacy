
const char* vertexShaderSource = R"""(
#version 330 core

layout (location = 0) in vec2 position;

out vec2 pos;

uniform mat4 projection;
uniform mat4 transform;
uniform mat4 localTransform;

void main(void) 
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
uniform vec2 start;
uniform vec2 end;
uniform int count;
uniform vec4 colors[16];
uniform float stops[16];
uniform int fillType;
in vec2 pos;
out vec4 fragColor;

void main()
{
    fragColor = vec4(0.3, 0.3, 0.3, 0.3);

    // if (fillType == 0)
    // {
    //     // solid
    //     gl_FragColor = vec4(color.rgb * color.a, color.a);
    // }
    // else if (fillType == 1)
    // {
    //     // linear
    //     vec2 toEnd = end - start;
    //     float lengthSquared = toEnd.x * toEnd.x + toEnd.y * toEnd.y;
    //     float f = dot(pos - start, toEnd) / lengthSquared;
    //     gl_FragColor =
    //         mix(colors[0], colors[1], smoothstep(stops[0], stops[1], f));
    //     for (int i = 1; i < 15; ++i)
    //     {
    //         if (i >= count - 1)
    //         {
    //             break;
    //         }
    //         gl_FragColor = mix(gl_FragColor,
    //                         colors[i + 1],
    //                         smoothstep(stops[i], stops[i + 1], f));
    //     }
    //     float alpha = gl_FragColor.w;
    //     gl_FragColor = vec4(gl_FragColor.xyz * alpha, alpha);
    // }
    // else if (fillType == 2)
    // {
    //     // radial
    //     float f = distance(start, pos) / distance(start, end);
    //     gl_FragColor =
    //         mix(colors[0], colors[1], smoothstep(stops[0], stops[1], f));
    //     for (int i = 1; i < 15; ++i)
    //     {
    //         if (i >= count - 1)
    //         {
    //             break;
    //         }
    //         gl_FragColor = mix(gl_FragColor,
    //                         colors[i + 1],
    //                         smoothstep(stops[i], stops[i + 1], f));
    //     }
    //     float alpha = gl_FragColor.w;
    //     gl_FragColor = vec4(gl_FragColor.xyz * alpha, alpha);
    // }
}
)""";