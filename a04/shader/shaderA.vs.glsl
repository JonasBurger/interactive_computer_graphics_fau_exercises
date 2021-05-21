#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

#pragma glio_include "spherical_coordinates.inc.glsl"

out vec2 lookup_coord;
void main()
{
    // transforms model- to view-normals:
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));

    // normal_view_space, pos_view_space is in Camera-space
    vec3 normal_view_space   = normalize(normalMatrix * vNormal.xyz);
    vec4 position_view_space = viewMatrix * modelMatrix * vec4(vPosition, 1);

    gl_Position  = projMatrix * position_view_space;
    lookup_coord = normal_view_space.xy;

    mat3 view_to_world = inverse(mat3(viewMatrix));

    // TODO: Calculate reflection vector in _view_ space
    vec3 camera_location_vector = normalize(-position_view_space.xyz/position_view_space.w);// vec3(0,0,-1);
    //vec3 carama_location_vector = vec3(0,0,-1);
    vec3 reflection_vector_view_space = 2*dot(camera_location_vector, normal_view_space)*normal_view_space - camera_location_vector;
    //vec3 halfway_vector_view_space = normalize(normalize(reflection_vector_view_space) + normalize(camera_location_vector));
    //vec3 reflection_vector_view_space = normal_view_space; // normal is reflection vector
    //vec3 reflection_vector_view_space = normalize(reflect(normalize(camera_location_vector), normal_view_space)); // normal is reflection vector

    // TODO: Transform reflection vector into _world_ space
    vec3 reflection_vector_world_space = view_to_world * reflection_vector_view_space;
    //vec3 halfway_vector_world_space = view_to_world * halfway_vector_view_space;

    // TODO: transform reflection vector into polar texture coordinates (see shperical_coordinates.inc.glsl)
    vec2 spherical_tc = SphericalCoordinates(normalize(reflection_vector_world_space));
    lookup_coord = spherical_tc;
    //lookup_coord = vec2(cos(spherical_tc.y)*sin(spherical_tc.x), sin(spherical_tc.y)*cos(spherical_tc.x));
    //lookup_coord = reflection_vector_world_space.xy;
}
