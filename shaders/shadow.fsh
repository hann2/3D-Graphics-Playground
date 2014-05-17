#version 330 core

// Interpolated values from the vertex shaders
in vec4 shadow_coord;
in vec3 fragment_normal;

// Ouput data
out vec3 frag_color;

// Values that stay constant for the whole mesh.
uniform sampler2D shadow_map;
uniform vec3 color;
uniform vec3 light_direction;
const vec3 light_color = vec3(1,1,1);

void main(){

    float visibility = texture(shadow_map, shadow_coord.xy).z;

    frag_color = visibility * -dot(light_direction, fragment_normal) * color * light_color;

}
