#version 330
  
layout(location = 0)  out vec4 out0; // color 

in vec2 tc;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D depth;

uniform vec3 lightDir;
uniform mat4 viewMatrix;
uniform int debug;

const float shinyness = 32.0;
const vec3 specColor = vec3(0.4, 0.4, 0.4);

void main() 
{ 

	vec3 col = vec3(0,0,0);
	vec2 TC = tc;

	if (debug == 1) {
		gl_FragDepth = -0.5;


		if (TC.y > 0.5) {
			TC.y -= 0.5;
			if (TC.x < 0.5) col = texture2D(diffuse, TC*vec2(2,2)).rgb;
			else			col = texture2D(normal, (TC-vec2(0.5,0))*vec2(2,2)).rgb;
		}
		else {
			if (TC.x < 0.5) col = vec3(texture2D(depth, TC*vec2(2,2)).r);
			else			col = texture2D(position, (TC-vec2(0.5,0))*vec2(2,2)).rgb;
		}
	} else {
		// TODO A1 (a)
		// Note that, the approximation of Phong lighting using the halfway vector (which is used by the reference)
		// yields slightly different results than regular Phong lighting

		
		// second try

    // retrieve data from G-buffer
    vec3 fragPos = texture2D(position, tc).rgb;
    vec3 normalVec = texture2D(normal, tc).rgb;
    vec3 originalColor = texture2D(diffuse, tc).rgb;
		vec3 viewPos = normalize(viewMatrix[3].xyz);

		//gl_FragDepth = fragPos.z;
		gl_FragDepth = texture2D(depth, tc).r;

		// ambient
		float ambientStrength = 1.0; // maybe reduce to make the note lighted parts of the scene look darker
		vec3 ambient = ambientStrength * specColor; // if i understand it correctly, diffuse stores the ambient color?

		// diffuse
		vec3 norm = normalize(normalVec);
		// already have lightDir (dont need to calculate)
		float diffuseStrengh = max(dot(norm, lightDir), 0.0);
		//vec3 diffuseColor = diffuseStrengh * specColor;
		vec3 diffuseColor = diffuseStrengh * specColor * 2; // stronger diffuse 

		// specular
		float specularStrengh = 0.5;
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shinyness);
		vec3 specular = specularStrengh * spec * specColor;

		// combine
		vec3 result = (ambient + diffuseColor + specular) * originalColor;
		col = result;


		/*
    // retrieve data from G-buffer
    vec3 FragPos = texture(position, tc).rgb;
    vec3 Normal = texture(normal, tc).rgb;
    vec3 Albedo = texture(diffuse, tc).rgb;
    float Specular = texture(diffuse, tc).a;

		// then calculate lighting as usual
    vec3 lighting = Albedo * 1.0; // hard-coded ambient component
    // vec3 viewDir = vec3(normalize(vec4(FragPos, 1.0) * viewMatrix));
		// diffuse
		//vec3 lightDir = normalize(lights[i].Position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * specColor;
		lighting += diffuse;
    col = lighting;
		*/

	}

	out0 = vec4(col, 1);
}
