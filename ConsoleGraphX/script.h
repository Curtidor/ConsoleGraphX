#pragma once
#include "component.h"

namespace ConsoleGraphX
{
    class Entity;

    /*
     * The Script class is a base class for all user-defined scripts that can be attached to entities in the game engine.
     * Scripts allow custom behaviors to be defined and executed for entities within a scene.
     *
     * Design Decision: Storing the Owner Entity Pointer in Scripts
     * -----------------------------------------------------------
     * In this engine architecture, each script stores a pointer to its owning entity. This decision was made for several reasons:
     *
     * 1. **Simplified Update Mechanism**: By storing the owner entity pointer, each script knows which entity it is attached to.
     *    This allows scripts to interact with their owner directly without needing to pass the entity pointer as a parameter
     *    to every function (`Awake`, `Start`, `Update`, etc.). This design reduces boilerplate code and keeps the script's API clean.
     *
     * 2. **Automatic Cleanup**: When an entity is destroyed, all of its components, including its scripts, are destroyed with it.
     *    This automatic cleanup mechanism ensures that there are no dangling pointers or memory leaks associated with scripts
     *    after an entity is deleted. It avoids the need for external systems (like a ScriptSystem) to track script lifetimes
     *    and removes the risk of accessing invalid memory.
     *
     * 3. **Encapsulation and Modularity**: By encapsulating the owner reference within the script, the script is self-contained
     *    and manages its relationship with its owner entity internally. This encapsulation makes the codebase more modular and
     *    easier to maintain, as the logic for script behavior remains within the script itself without relying on other systems
     *    to provide necessary context (like the owner entity).
     *
     * 4. **Efficient Script Management**: The previous design required a separate system to maintain a list of entities that
     *    had scripts attached, adding complexity and potential inefficiencies to the update process. By moving ownership
     *    management directly into the scripts, the `ScriptSystem` can iterate over all scripts directly without needing to
     *    track entity-script relationships externally. This reduces the overhead associated with script updates, especially
     *    when not all entities have scripts.
     *
     * 5. **Flexible Interaction**: With the owner entity pointer readily available, scripts can easily interact with other
     *    components of their entity. For instance, a `PlayerController` script can quickly access the `Transform` or `Physics`
     *    components of its owning entity, enhancing the flexibility and interactivity of scripts within the engine.
     *
     * This design approach ensures a clean, efficient, and robust mechanism for handling scripts within the game engine,
     * aligning with the engine's goals of high performance, modularity, and ease of use.
     */

    struct Script: public ConsoleGraphX_Internal::Component
    {
    protected:
        Entity* _m_owner;
        bool _m_isEnabled;

    public:
        Script();
        Script(Entity* owner);
        Script(const Script& script, Entity* owner);

        void SetOwner(Entity* owner);
        const Entity* GetOwner() const;
        
        virtual void Awake();
        virtual void Start();
        virtual void Update(float delta_time);

        bool IsEnabled() const;
        void SetState(bool state);

        virtual void Clone(Script*& script);
        virtual void Clone(Script*& script, Entity* owner);
    };
};
