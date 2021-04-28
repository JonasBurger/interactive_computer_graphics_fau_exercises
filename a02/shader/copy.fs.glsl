#version 330
in vec2 tc;
uniform sampler2D depth;

void main()
{
    gl_FragDepth = texture2D(depth, tc).r;
}
