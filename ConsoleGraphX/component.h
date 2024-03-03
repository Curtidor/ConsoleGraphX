#pragma once
#include <string>

/*
Base class for all components
*/
struct Component
{
public:
    Component();

    virtual ~Component();
    virtual Component* Clone() const = 0;
    virtual int GetID() const = 0;

    const std::string ComponentName() const;
};


enum ComponentID
{
    base = 0,
    playerController = 1,
    script = 2,
    sprite = 3,
    transform = 4,
};