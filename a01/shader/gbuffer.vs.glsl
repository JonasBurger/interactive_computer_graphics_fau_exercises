#version 330
  
layout(location = 0) in  vec3 vPosition; 
layout(location = 1) in  vec3 vNormal; 

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

out vec3 normal;
out vec4 pos_ec;

void main() {
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));

	normal = normalMatrix * vNormal.xyz;
	pos_ec = viewMatrix * modelMatrix * vec4(vPosition, 1);
	gl_Position = projMatrix * pos_ec;
}
