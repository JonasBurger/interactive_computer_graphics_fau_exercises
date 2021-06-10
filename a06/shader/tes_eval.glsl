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
    vec3 posBottom = mix(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_TessCoord.x);
    vec3 posTop = mix(gl_in[3].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_TessCoord.x);
    vec3 pos = mix(posBottom, posTop, gl_TessCoord.y);





    // TODO: b) use heightmap to adjust height



    height      = pos.y;
    position    = (model * vec4(pos, 1.0)).xyz;
    gl_Position = projection * view * vec4(position, 1.0);

    // TODO: b) calculate the normal of the vertex
    normal = vec3(0, 1, 0);






































}
