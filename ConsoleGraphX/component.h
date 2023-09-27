#pragma once

/*
Base class for all components
*/
struct Component
{
public:
    Component();
    virtual ~Component();

    virtual int GetID() const;
};


enum ComponentID
{
    base = 0,
    physics_body2d = 1,
    player_controller = 2,
    script = 3,
    sprite = 4,
    transform = 5
};