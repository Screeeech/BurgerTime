//
// Created by lily-laptop on 18/02/2026.
//

#ifndef MINIGIN_COMPONENT_H
#define MINIGIN_COMPONENT_H

class Component
{
public:
    virtual ~Component() = default;
    Component() = default;
    virtual void receive(int message) = 0;
    virtual void update() = 0;
};


#endif  // MINIGIN_COMPONENT_H
