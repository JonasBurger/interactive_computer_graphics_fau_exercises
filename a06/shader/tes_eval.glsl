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
    vec3 posBL = gl_in[0].gl_Position.xyz;
    vec3 posBR = gl_in[1].gl_Position.xyz;
    vec3 posTL = gl_in[3].gl_Position.xyz;
    vec3 posTR = gl_in[2].gl_Position.xyz;

    vec3 posBottom = mix(posBL, posBR, gl_TessCoord.x);
    vec3 posTop = mix(posTL, posTR, gl_TessCoord.x);
    vec3 pos = mix(posBottom, posTop, gl_TessCoord.y);





    // TODO: b) use heightmap to adjust height
    pos.y = texture(heightmap, pos.xz).r * height_scale;




    height      = pos.y;
    position    = (model * vec4(pos, 1.0)).xyz;
    gl_Position = projection * view * vec4(position, 1.0);

    // TODO: b) calculate the normal of the vertex

    // adjust pos* from highmap too
    posBL.y = texture(heightmap, posBL.xz).r * height_scale;
    posBR.y = texture(heightmap, posBR.xz).r * height_scale;
    posTL.y = texture(heightmap, posTL.xz).r * height_scale;
    posTR.y = texture(heightmap, posTR.xz).r * height_scale;

    vec3 posTobl = posBL - pos;
    vec3 posTobr = posBR - pos;
    vec3 posTotl = posTL - pos;
    vec3 posTotr = posTR - pos;
    //if(posTobl == vec3(0.f)) posTobl=vec3(-1, 0, -1);
    //if(posTobr == vec3(0.f)) posTobr=vec3(1, 0, -1);
    //if(posTotl == vec3(0.f)) posTotl=vec3(-1, 0, 1);
    //if(posTotr == vec3(0.f)) posTotr=vec3(1, 0, 1);
    
    vec3 normalLeft = cross(posTobl, posTobr);
    vec3 normalRight = cross(posTotr, posTotl);
    normal = -normalize(normalize(normalLeft) + normalize(normalRight));

    // TODO: fix normal calculation

    normal = vec3(0, 1, 0); 
    //U = 
    //normal = 






































}
