#version 330
  
layout(location = 0)  out vec4 out0; // color 

in vec4 v_center;

uniform uvec2 resolution;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;

uniform float radius;
uniform vec3 splatColor;
uniform vec3 attenuation_params;
uniform float light_intensity;

void main() 
{ 
	vec2 tc = gl_FragCoord.xy / resolution;

	// distance
	vec3 fragPos = texture2D(position, tc).xyz;
	vec3 lightPos = v_center.xyz / v_center.w;
	float distance = length(fragPos - lightPos);
	float normDistance = distance / radius;

	if (normDistance > 1){
		discard;
	}
	
	// attenuation
	float a = attenuation_params.x;
	float b = attenuation_params.y;
	float c = attenuation_params.z;
	float x = normDistance;
	float attenuation = 1/(a+b*x+c*x*x)-1/(a+b+c);

	// normal lighting stuff
	vec3 fragNormal = texture2D(normal, tc).xyz;
	vec3 fragOriginalColor = texture2D(diffuse, tc).rgb;
	vec3 lightOriginDirection = normalize(lightPos - fragPos);
	float diffuseStrengh = max(dot(fragNormal, lightOriginDirection), 0.0);

	// combine
	out0 = vec4(splatColor * attenuation * diffuseStrengh * light_intensity, 1.0);
	//out0 = vec4(1,1,1,1);

	// debug light source position
	// if (length(fragPos.xy - lightPos.xy)  < 0.1){
	// 	out0 = vec4(splatColor*vec3(10.,10.,10.), 1.0);
	// }


	// TODO (b):
	// - gbuffer position in splat radius?
	// - diffuse shading
	// - distance attenuation

	//out0 = vec4(splatColor, 1.0);
























}
