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
    return length(v0 - v1) * tess_factor;


}

vec3 ndc(vec4 v) {
    v = projection * view * model * v;
    return v.xyz / v.w;
}

void main()
{
    if (gl_InvocationID == 0)
    {
        if (!use_lod)
        {
            // TODO: a) set gl_TessLevel... according to tess_const
            gl_TessLevelInner[0] = gl_TessLevelInner[1] =
            gl_TessLevelOuter[0] = gl_TessLevelOuter[1] =
            gl_TessLevelOuter[2] = gl_TessLevelOuter[3] = tess_const;


        }
        else
        {
            // TODO: c) calculate normalized device coordinates (v0-3) of the vertices
            vec3 v0 = ndc(gl_in[0].gl_Position);
            vec3 v1 = ndc(gl_in[1].gl_Position);
            vec3 v2 = ndc(gl_in[2].gl_Position);
            vec3 v3 = ndc(gl_in[3].gl_Position);

            if (offscreen(v0, v1, v2, v3))
            {
                // TODO: c) dump quad
                gl_TessLevelOuter[0] = 0; // kinda hacky
                // Note: If any of the outer levels used by the abstract patch type is 0 or negative (or NaN), then the patch will be discarded by the generator, and no TES invocations for this patch will result.
                return;
            }
            else
            {
                vec2 ss0 = screen_space(v0);
                vec2 ss1 = screen_space(v1);
                vec2 ss2 = screen_space(v2);
                vec2 ss3 = screen_space(v3);

                // TODO: c) set gl_TessLevel...
                // Note ssX and the edge_lod-function


                gl_TessLevelOuter[0] = max(edge_lod(ss3, ss0), 1.f);
                gl_TessLevelOuter[1] = max(edge_lod(ss0, ss1), 1.f);
                gl_TessLevelOuter[2] = max(edge_lod(ss1, ss2), 1.f);
                gl_TessLevelOuter[3] = max(edge_lod(ss2, ss3), 1.f);


                gl_TessLevelInner[0] = round((gl_TessLevelOuter[1] + gl_TessLevelOuter[3])/2+0.5);
                gl_TessLevelInner[1] = round((gl_TessLevelOuter[0] + gl_TessLevelOuter[2])/2+0.5);




















            }
        }
    }

    // pass through position
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
