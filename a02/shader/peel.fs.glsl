#version 330

layout(location = 0) out vec4 out0;  // color

in vec3 N;
in vec3 L;
in vec4 P;
in vec2 tc;

uniform vec4 color;
uniform sampler2D minDepth;

const float shinyness  = 32.0;
const vec3 ambientCol  = vec3(0.1, 0.1, 0.2);  // grey
const vec3 specularCol = vec3(0.4, 0.4, 0.4);  // white


vec3 blinn_phong_reflectance(vec3 N, vec3 L, vec3 V)
{
    vec3 H = normalize(L + V);  // half way vector

    vec3 amb = ambientCol;

    if (dot(L, N) < 0) N = -N;

    vec3 diff = max(0.0, dot(L, N)) * color.rgb;
    vec3 spec = pow(max(0.0, dot(H, N)), shinyness) * specularCol;

    return amb + diff + spec;
}


void main()
{
    if (gl_FragCoord.z <= texture2D(minDepth, tc).r) discard;

    vec3 lDir    = normalize(L);
    vec3 normal  = normalize(N);
    vec3 viewDir = normalize(-P.xyz);

    out0 = vec4(blinn_phong_reflectance(normal, lDir, viewDir), color.a);
}
