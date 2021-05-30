#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform mat4 shadowProj;
uniform vec3 lightSource;

out vec3 N;
out vec4 L;
out vec4 P;
out vec4 shadow_tc;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));

    N = normalMatrix * vNormal.xyz;
    L = viewMatrix * vec4(lightSource, 1);
    P = viewMatrix * modelMatrix * vec4(vPosition, 1);

    // TODO:  a) appropriate shadow mapping transformation.
    vec4 shadowDC = shadowProj * modelMatrix * vec4(vPosition, 1);
    vec3 shadowNDC = shadowDC.xyz / shadowDC.w;
    shadow_tc = vec4(shadowNDC.xy / 2 + 0.5, shadowNDC.z, 1);
    //shadow_tc = shadowDC / shadowDC.w;


    gl_Position = projMatrix * P;
}
