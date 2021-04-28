#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 lightDir;

out vec3 N;
out vec3 L;
out vec4 P;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));

    N = normalMatrix * vNormal;
    L = (viewMatrix * vec4(-lightDir, 0.f)).xyz;
    P = viewMatrix * modelMatrix * vec4(vPosition, 1.0);

    gl_Position = projMatrix * P;
}
