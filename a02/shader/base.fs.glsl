#version 330

layout(location = 0) out vec4 out0;  // color
in vec2 tc;

uniform sampler2D color;
uniform sampler2D depth;

void main()
{
    out0         = texture2D(color, tc);
    gl_FragDepth = texture2D(depth, tc).r;
}
