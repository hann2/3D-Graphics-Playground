
#ifndef _TURTLE_H
#define _TURTLE_H

#include <glm/glm.hpp>
#include <vector>

class Turtle {
    public:
        Turtle();
        void pitch(float angle);
        void roll(float angle);
        void yaw(float angle);
        void forward(float ammount);
        void push_state();
        void pop_state();
        glm::mat4 * peek_state();
    private:
        std::vector<glm::mat4 *> turtle_state_stack;
        glm::mat4 * turtle_state;
};


#endif

