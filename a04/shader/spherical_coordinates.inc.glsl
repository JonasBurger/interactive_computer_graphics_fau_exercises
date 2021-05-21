
uniform bool cylindricMapping;
const float PI = 3.1415927;

vec2 SphericalCoordinates(vec3 r)
{
    //vec2 lookup_coord = r.xy;
    // TODO: a) transform reflection vector into polar texture coordinates
    //vec2 tc = (lookup_coord + vec2(1,1))/2;
    //vec2 lookup_coord = vec2(acos(r.z / sqrt(r.x*r.x+r.y*r.y+r.z*r.z)), atan(r.y/r.x));
    float m = 2*sqrt(pow(r.x, 2)+pow(r.y, 2)+pow(r.z+1, 2));
    vec2 lookup_coord = vec2(r.x/m+0.5, r.y/m+0.5);

    return lookup_coord;
}