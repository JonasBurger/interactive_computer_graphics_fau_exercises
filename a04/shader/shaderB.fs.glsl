#version 330

layout(location = 0) out vec4 out0;  // color

in vec3 normal_view_space;
in vec3 position_view_space;
in mat3 view_to_world;

uniform sampler2D envMap;
uniform bool debugUV;

#pragma glio_include "spherical_coordinates.inc.glsl"

void main()
{
    vec3 N            = normalize(normal_view_space);
    vec2 lookup_coord = N.xy;

    // TODO: Calculate reflection vector in _view_ space
    vec3 camera_location_vector = normalize(-position_view_space.xyz);// vec3(0,0,-1);
    //vec3 reflection_vector_view_space = 2*dot(camera_location_vector, N)*N - camera_location_vector;
    //vec3 reflection_vector_view_space = normal_view_space;
    vec3 reflection_vector_view_space = reflect(-camera_location_vector, N);
    // TODO: Transform reflection vector into _world_ space
    vec3 reflection_vector_world_space = view_to_world * reflection_vector_view_space;


    // TODO: transform reflection vector into polar texture coordinates (see shperical_coordinates.inc.glsl)
    lookup_coord = SphericalCoordinates(normalize(reflection_vector_world_space));
    //lookup_coord = vec2(lookup_coord.x < 0?-lookup_coord.x:0, lookup_coord.x >= 0?lookup_coord.x:0);

    //lookup_coord = vec2(max(0, reflection_vector_view_space.x / reflection_vector_view_space.z), 0);
    //lookup_coord = vec2(N.x / N.z, 0);

    //lookup_coord = vec2(reflection_vector_view_space.z, 0);
    //lookup_coord = vec2(max(0, reflection_vector_world_space.z), 0);
    //lookup_coord = vec2(N.z, 0);;

    //lookup_coord = vec2(max(0, reflection_vector_world_space.x), 0);
    //lookup_coord = vec2(reflection_vector_view_space.x, 0);
    //lookup_coord = vec2(N.z, 0);;


    out0 = vec4( debugUV ? vec3(lookup_coord, 0) 
						 : texture2D(envMap, lookup_coord).rgb
			   , 1);
}
