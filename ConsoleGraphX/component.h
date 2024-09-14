#pragma once

/*
Base class for all components
*/
namespace ConsoleGraphX_Internal
{
    struct Component
    {
    public:
        Component();

        virtual ~Component() = default;

        void Clone(Component* component) const;
    };
};