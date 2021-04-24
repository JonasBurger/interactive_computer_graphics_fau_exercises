#version 330
  
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPos;

uniform vec3 color;

in vec3 normal;
in vec4 pos_ec;
 

void main() 
{ 
	outColor = vec4(color, 1);
	outNormal = vec4(normalize(normal), 1);
	outPos = vec4(pos_ec.xyz/pos_ec.w, 1);
}
