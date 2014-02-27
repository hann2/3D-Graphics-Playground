
#include "Turtle.h"
#include <glm/gtc/matrix_transform.hpp>

Turtle::Turtle() {
    turtle_state_stack = std::vector<glm::mat4 *>();
    turtle_state = new glm::mat4();
}

void Turtle::pitch(float angle) {
    *turtle_state = glm::rotate(*turtle_state, angle, glm::vec3(0.0, 1.0, 0.0));
}

void Turtle::roll(float angle) {
    *turtle_state = glm::rotate(*turtle_state, angle, glm::vec3(0.0, 0.0, 1.0));
}

void Turtle::yaw(float angle) {
    *turtle_state = glm::rotate(*turtle_state, angle, glm::vec3(1.0, 0.0, 0.0));
}

void Turtle::forward(float amount) {
    *turtle_state = glm::translate(*turtle_state, glm::vec3(0, 0, amount));
}

void Turtle::push_state() {
    turtle_state_stack.push_back(new glm::mat4(*turtle_state));
}

void Turtle::pop_state() {
    delete turtle_state;
    turtle_state = turtle_state_stack.back();
    turtle_state_stack.pop_back();
}

glm::mat4 * Turtle::peek_state() {
    return new glm::mat4(*turtle_state);
}

