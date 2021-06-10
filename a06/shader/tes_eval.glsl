#version 420
layout(quads) in;

out float height;
out vec3 position;
out vec3 normal;

uniform sampler2D heightmap;
uniform float height_scale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // TODO: a) interpolate position
    vec3 pos = gl_in[1].gl_Position.xyz;














    // TODO: b) use heightmap to adjust height



    height      = pos.y;
    position    = (model * vec4(pos, 1.0)).xyz;
    gl_Position = projection * view * vec4(position, 1.0);

    // TODO: b) calculate the normal of the vertex
    normal = vec3(0, 1, 0);






































}
