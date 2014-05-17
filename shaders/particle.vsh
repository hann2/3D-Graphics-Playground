
#version 330 core

in vec3 initial_velocity;
in float time_offset;

uniform float t;

const vec3 start_pos = vec3(0, 0.75, 0.2);
const vec3 acceleration = vec3(0, 9.8, 0);
const float end_height = 4;
const float period = sqrt(2 * (end_height - start_pos.y) / acceleration.y);

out vec3 velocity;
out vec3 pos;

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;

void main() {
    float temp = t * 0.0008 + time_offset;
    float time = (temp - floor(temp)) * period;
    velocity = acceleration * time + initial_velocity;
    pos = start_pos + initial_velocity * time + acceleration * time * time / 2;

    gl_Position = perspective_transform * view_transform * model_transform * vec4(pos, 1);
}
