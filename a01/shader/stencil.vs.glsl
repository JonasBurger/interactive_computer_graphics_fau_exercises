#version 330
layout(location = 0) in  vec3 vPosition; 
layout(location = 1) in  vec3 vNormal; 

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1);
}
