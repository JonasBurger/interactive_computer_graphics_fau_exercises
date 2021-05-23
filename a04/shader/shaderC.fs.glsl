#version 330

layout(location = 0) out vec4 out0;  // color

in vec3 normal_view_space;
in vec3 position_view_space;
in mat3 view_to_world;

uniform sampler2D envMap;
uniform bool debugUV;

#pragma glio_include "spherical_coordinates.inc.glsl"

// uniform distribution in [0 .. 1], pretty good numbers,
// inspired by http://lumina.sourceforge.net/Tutorials/Noise.html
float random_uniform(vec2 init, int seed)
{
    //return 0.5;
    int x = int(1048576.0 * init.x);
    int y = int(1048576.0 * init.y);
    int n = x + y * 789221 + seed;
    n     = (n << 13) ^ n;
    return 1.0 - 0.5 * float((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

vec2 reflection(vec3 facet_normal_view_space)
{
    vec2 lookup_coord = facet_normal_view_space.xy;
    // TODO: Calculate reflection vector in _view_ space
    vec3 camera_location_vector = normalize(-position_view_space.xyz);
    vec3 reflection_vector_view_space = 2*dot(camera_location_vector, normal_view_space)*normal_view_space - camera_location_vector;

    // TODO: Transform reflection vector into _world_ space
    vec3 reflection_vector_world_space = view_to_world * reflection_vector_view_space;




    // TODO: transform reflection vector into polar texture coordinates (see shperical_coordinates.inc.glsl)
    lookup_coord = SphericalCoordinates(normalize(reflection_vector_world_space));
    return lookup_coord;
}


uniform float roughness;
uniform int glossyRays;


void main()
{
    vec3 N          = normalize(normal_view_space);
    vec3 refl_color = vec3(0, 0, 0);

    // TODO: Take glossyRays reflected samples, note the texture() call below.
    // The bias is used to get accurate samples and to bypass the texture mipmap level (wich otherwise results in the
    // brown visible pixel-seam)
    vec3 T = normalize(cross(N, normalize(N+vec3(1,1,1)))); // there's probably a nicer way
    vec3 B = normalize(cross(N, T));

    for(int i = 0; i < glossyRays; ++i){
        vec3 n_tangent_space = vec3(1,0,0);
        vec2 u = vec2(random_uniform(N.xy, i*2), random_uniform(N.xy, i*2+1)); // parameters are kinda random
        float z0 = sqrt(-2*log(u.x))*cos(2*PI*u.y);
        float z1 = sqrt(-2*log(u.x))*sin(2*PI*u.y);
        n_tangent_space.y = z0*roughness;
        n_tangent_space.z = z1*roughness;
        vec3 n_view_space = normalize(n_tangent_space.x * N + n_tangent_space.y * T + n_tangent_space.z * B);

        vec2 lookup_coord = reflection(n_view_space);
        refl_color += (debugUV ? vec3(lookup_coord,0) : texture(envMap, lookup_coord, -30).rgb) / glossyRays;
    }






























    out0 = vec4(refl_color.xyz, 1.0);
}
