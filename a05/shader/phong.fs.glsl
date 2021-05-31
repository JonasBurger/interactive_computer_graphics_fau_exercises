#version 330

layout(location = 0) out vec4 out0;  // color

in vec3 N;
in vec4 L;
in vec4 P;
in vec4 shadow_tc;

uniform sampler2DShadow shadow_map;
uniform sampler2D light_shape;
uniform vec3 color;
uniform int pcf_kernel_size;

const float shinyness   = 32.0;
const vec3 ambient_col  = vec3(0.1, 0.1, 0.2);  // grey
const vec3 specular_col = vec3(0.4, 0.4, 0.4);  // white

vec4 blinn_phong_reflectance(vec3 N, vec3 L, vec3 V, vec3 light_col, float is_lit)
{
    vec3 H = normalize(L + V);  // half way vector

    vec3 amb = color * ambient_col;

    vec3 diff = is_lit * max(0.0, dot(L, N)) * color * light_col;
    vec3 spec = is_lit * pow(max(0.0, dot(H, N)), shinyness) * specular_col * light_col;

    return vec4(amb + diff + spec, 1.0);
}

void main()
{
    vec3 lightDir = normalize(L.xyz - P.xyz);
    vec3 viewDir  = normalize(-P.xyz);
    vec3 normal   = normalize(N);

    vec3 light_color = vec3(1);
    // TODO: enable me.
    light_color = textureProj(light_shape, shadow_tc.xyz).xyz;

    // TODO:  a)  read shadow-map
    float poligon_offset = -0.0005;
    float is_lit = texture(shadow_map, shadow_tc.xyz/shadow_tc.xyz + vec3(0,0,poligon_offset));






    // TODO:  b)  do PCF


    float sampleSize = pow(pcf_kernel_size*2-1, 2);
    float sum = 0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for (int x = -pcf_kernel_size+1; x < pcf_kernel_size; x++){
        for (int y = -pcf_kernel_size+1; y < pcf_kernel_size; y++){
            sum += texture(shadow_map, shadow_tc.xyz + vec3(x*texelSize.x,y*texelSize.y,0) + vec3(0,0,poligon_offset)) / sampleSize; // same depth?
            // sum += textureOffset(shadow_map, shadow_tc.xyz, ivec2(x,y)) / sampleSize; // same depth?
        }
    }     
    is_lit = sum; 











    out0 = blinn_phong_reflectance(normal, lightDir, viewDir, light_color, is_lit);
}
