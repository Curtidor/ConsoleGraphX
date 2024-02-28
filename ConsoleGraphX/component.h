#pragma once

/*
Base class for all components
*/
struct Component
{
public:
    Component();

    virtual ~Component();
    virtual int GetID() const = 0;
};


enum ComponentID
{
    base = 0,
    playerController = 1,
    script = 2,
    sprite = 3,
    transform = 4,
    spawner = 5
};