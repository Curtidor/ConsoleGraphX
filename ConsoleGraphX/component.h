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
    player_controller = 1,
    script = 2,
    sprite = 3,
    transform = 4
};