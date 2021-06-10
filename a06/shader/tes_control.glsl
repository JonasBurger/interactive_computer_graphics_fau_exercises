#version 420
layout(vertices = 4) out;

uniform float tess_const;
uniform float tess_factor;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform bool use_lod;
uniform uvec2 resolution;

// works in normalized device coordinates [-1,1]**3, does not cover all cases
bool offscreen(vec3 v0, vec3 v1, vec3 v2, vec3 v3)
{
    // only covers quad visibility + margin (could be improved using bounding-box visibility)
    float margin = 2;
    if (v0.x < -margin && v1.x < -margin && v2.x < -margin && v3.x < -margin) return true;  // all left
    if (v0.x >  margin && v1.x >  margin && v2.x >  margin && v3.x >  margin) return true;  // all right
    if (v0.y < -margin && v1.y < -margin && v2.y < -margin && v3.y < -margin) return true;  // all below
    if (v0.y >  margin && v1.y >  margin && v2.y >  margin && v3.y >  margin) return true;  // all above
    if (v0.z < -margin && v1.z < -margin && v2.z < -margin && v3.z < -margin) return true;  // all in front of
    if (v0.z >  margin && v1.z >  margin && v2.z >  margin && v3.z >  margin) return true;  // all behind
    return false;
}

// converts normalized device coordinates to clamped screen coordinates
vec2 screen_space(vec3 vertex)
{
    return (clamp(vertex.xy, -1.3, 1.3) + 1) * (vec2(resolution) * 0.5);
}

float edge_lod(vec2 v0, vec2 v1)
{
    // TODO: c) calculate the tesselation level according to edge length in pixel (scaled by tess_factor)
    return 1;


}

void main()
{
    if (gl_InvocationID == 0)
    {
        if (!use_lod)
        {
            // TODO: a) set gl_TessLevel... according to tess_const






        }
        else
        {
            // TODO: c) calculate normalized device coordinates (v0-3) of the vertices
            vec3 v0;
            vec3 v1;
            vec3 v2;
            vec3 v3;









            if (offscreen(v0, v1, v2, v3))
            {
                // TODO: c) dump quad






            }
            else
            {
                vec2 ss0 = screen_space(v0);
                vec2 ss1 = screen_space(v1);
                vec2 ss2 = screen_space(v2);
                vec2 ss3 = screen_space(v3);

                // TODO: c) set gl_TessLevel...
                // Note ssX and the edge_lod-function


























            }
        }
    }

    // pass through position
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
