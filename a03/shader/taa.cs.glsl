#version 450

#define gid gl_GlobalInvocationID.xy
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0) uniform sampler2D currFrame;
layout(binding = 1) uniform sampler2D history;
layout(binding = 2, rgba32f) uniform image2D taaOut;

uniform bool init;
uniform uvec2 res;

uniform float[9] weights;
uniform mat4 reProj;

uniform bool freeze;
uniform bool doFilter;
uniform bool doClamp;
uniform bool doDynamicFeedback;
uniform float maxFeedback;

const ivec2 offsets[9] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
float luma(vec3 rgb)
{
    return 0.299 * rgb.r + 0.578 * rgb.g + 0.144 * rgb.b;
}

vec3 vmax(vec3 v1, vec3 v2){
    return vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));
}

vec3 vmin(vec3 v1, vec3 v2){
    return vec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));
}

void main()
{
    if (gid.x >= res.x || gid.y >= res.y) return;

    // get filtered sample and color neighborhood
    vec4 color       = vec4(0);
    vec3 colorBoxMax = vec3(-1.0 / 0.0);  // -infinity
    vec3 colorBoxMin = vec3(1.0 / 0.0);   // +infinity

    vec3 neighborhoodColorMax = vec3(0.f, 0.f, 0.f);
    vec3 neighborhoodColorMin = vec3(1.f, 1.f, 1.f);

    for (int i = 0; i < 9; ++i)
    {
        // TODO b) accumulate weighed 3x3 samples from current frame
        vec4 pixelColor = texelFetch(currFrame, ivec2(gid) + offsets[i], 0);


        color += pixelColor * weights[i];


        // TODO d) calculate absolute 3x3 neighborhood min/max and clamp history sample
        neighborhoodColorMin = vmin(neighborhoodColorMin, pixelColor.rgb);
        neighborhoodColorMax = vmax(neighborhoodColorMax, pixelColor.rgb);
    }

    // if !doFilter use unweighted center pixel
    if (!doFilter) color = texelFetch(currFrame, ivec2(gid) + offsets[4], 0);

    // reprojection
    vec2 tex_coord = (vec2(gid) + 0.5) / res;
    // here, depth is between 0 and 1
    float depth = texelFetch(currFrame, ivec2(gid), 0).a;

    vec2 prev_tex_coord = tex_coord;
    // TODO c) reproject current screen space texture coordinates to coordinates from previous frame
    
    // what is depth exactly?
    vec2 texCordCannonical = tex_coord * 2.0 - 1.0;
    vec4 texCoordForReprojection = vec4(texCordCannonical.x, texCordCannonical.y, depth * 2.0 - 1.0, 1.0);
    vec4 q_cs = reProj * texCoordForReprojection;
    prev_tex_coord = vec2( q_cs / q_cs.w ) / 2 + 0.5;
    //vec4 fragCoord = gl_FragCoord.xyzw;
    //vec4 q_cs = reProj * fragCoord;
    //prev_tex_coord = 0.5 * ( q_cs.xy / q_cs.w ) + 0.5;




    // no history available (init) or out-of-bounds access
    vec4 historyColor = texture(history, prev_tex_coord);
    if (init || prev_tex_coord != clamp(prev_tex_coord, 0, 1)) historyColor.xyz = color.xyz;

    // history clamping
    if (doClamp)
    {
        // TODO d) clamp history to min/max
        historyColor.rgb = vmin(vmax(historyColor.rgb, neighborhoodColorMin), neighborhoodColorMax);

    }

    float feedback = maxFeedback;
    // dynamic feedback
    if (doDynamicFeedback)
    {
        // bonus: adjust feedback depending on luma/velocity






    }

    // TODO a) mix current frame with previous frames (feedback)
    float alpha = maxFeedback;
    vec4 newColor = color * (1.f - alpha) + historyColor * alpha;
    imageStore(taaOut, ivec2(gid), newColor);

        //////////////////////////////////////////////
    if (freeze) imageStore(taaOut, ivec2(gid), texelFetch(history, ivec2(gid), 0));
}
