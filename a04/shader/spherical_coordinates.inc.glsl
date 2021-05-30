
uniform bool cylindricMapping;
const float PI = 3.1415927;

float angleFromVec(vec2 v){
    //if(v.x > 0){
    //    return atan(v.y, v.x);
    //}
    float angleRaw = atan(abs(v.y), abs(v.x));
    if(v.x >= 0 && v.y >= 0){
        return angleRaw;
    }else if(v.x < 0 && v.y >= 0){
        return PI-angleRaw;
    }else if(v.x < 0 && v.y < 0){
        return PI+angleRaw;
    }else if(v.x >= 0 && v.y < 0){
        return 2*PI-angleRaw;
    }
    return 1/0.f; // guru meditation
}

vec2 SphericalCoordinates(vec3 r)
{
    //vec2 lookup_coord = r.xy;
    // TODO: a) transform reflection vector into polar texture coordinates
    //vec2 tc = (lookup_coord + vec2(1,1))/2;
    //vec3 i = vec3(r.y, r.z, r.x); // r.yzx; // iso coords
    //vec3 i = r.zxy;
    vec3 i = vec3(r.z, r.x, r.y);
    float inclination = acos(i.z / sqrt(i.x*i.x+i.y*i.y+i.z*i.z));
    float azimuth = atan(i.y, i.x);
    //float azimuth = atan(i.y/ i.x); // not correct but better for debugging :D
    //float azimuth = atan(i.y/ i.x);  // previous
    //float azimuth = angleFromVec(vec2(i.x, i.y));
     //float azimuth = atan(i.x/ i.y);

    vec2 lookup_coord = vec2(0);
    //lookup_coord.x = azimuth / (2*PI);
    //lookup_coord.y = (inclination+PI/2)/PI;
    //lookup_coord.x = (azimuth+PI/2)/PI;
    lookup_coord.x = azimuth/(2*PI)-0.25; // neue variante
    lookup_coord.y = 1-inclination / PI;
    //lookup_coord = lookup_coord.yx;
    //float m = 2*sqrt(pow(r.x, 2)+pow(r.y, 2)+pow(r.z+1, 2));
    //vec2 lookup_coord = vec2(r.x/m+0.5, r.y/m+0.5);
    //lookup_coord=clamp(lookup_coord, vec2(0,0), vec2(1,1));

    return lookup_coord;
}