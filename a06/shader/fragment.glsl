#version 420

layout(location = 0) out vec4 out0;  // color

uniform bool wireframe;
uniform vec3 light_dir;
uniform float watermax;
uniform float sandmax;
uniform float grassmax;
uniform float stonemax;
uniform float snowmax;
uniform vec3 cam_pos;

in float height;
in vec3 position;
in vec3 normal;

const vec4 water_col = vec4(0.1, 0.4, 1.0, 1.0);
const vec4 sand_col  = vec4(217 / 255.0, 180 / 255.0, 109 / 255.0, 0.01);
const vec4 grass_col = vec4(18 / 255.0, 159 / 255.0, 9 / 255.0, 0.4);
const vec4 stone_col = vec4(164 / 255.0, 154 / 255.0, 154 / 255.0, 0.01);
const vec4 snow_col  = vec4(1.0, 1.0, 1.0, 0.1);

void main(void)
{
    if (wireframe)
    {
        out0 = vec4(0.8, 0.8, 0.8, 1.0);
    }
    else
    {
        vec3 N = normalize(normal);
        vec3 L = -light_dir;  //  already normalized
        vec3 V = normalize(cam_pos - position);
        vec3 H = normalize(L + V);  // half way vector

        vec4 dc;
        if (height <= watermax)
            dc = water_col;
        else if (height <= sandmax)
            dc = mix(water_col, sand_col, smoothstep(watermax, sandmax, height));
        else if (height <= grassmax)
            dc = mix(sand_col, grass_col, smoothstep(sandmax, grassmax, height));
        else if (height <= stonemax)
            dc = mix(grass_col, stone_col, smoothstep(grassmax, stonemax, height));
        else
            dc = mix(stone_col, snow_col, smoothstep(stonemax, snowmax, height));

        vec3 amb  = dc.xyz * 0.2;
        vec3 diff = dc.xyz * 0.8 * max(0.0, dot(L, N));
        vec3 spec = dc.a * 0.5 * vec3(1, 1, 1) * pow(max(0.0, dot(H, N)), 64);
        out0      = vec4(amb + diff + spec, 1.0);
    }
}
