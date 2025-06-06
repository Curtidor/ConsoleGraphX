# Scene Lifetime Management Across DLL Boundaries

## Overview

This document explains the architecture decision made regarding **scene lifetime management** in this engine, particularly when dealing with **user-defined game logic loaded via DLLs**. It describes how scene memory is allocated, who controls its lifetime, and why this structure avoids common issues such as **heap mismatches**.

---

## Why This Matters

This engine is designed to load user-written games or simulations through a **DLL plugin system**. Users define entities, components, and behavior scripts in the DLL. The **core engine (EXE)** handles high-level orchestration: windowing, rendering, physics, and so on.

However, when you mix memory allocation and deallocation across **module boundaries** (EXE <-> DLL), you can run into **heap mismatch issues**—particularly on Windows where each module may use a separate memory heap and runtime library.

To ensure a robust and safe shutdown, we need a well-defined ownership and destruction strategy.

---

## The Problem

Currently, the **scene**—a central object containing:

- An unordered set of entities,
- A resource manager (textures, sprites, scripts, colliders, etc),

is allocated within the DLL. This is necessary because:

- The DLL defines the **types** for user-created scripts and components (e.g., `PlayerController`, `EnemyAI`),
- These types are unknown to the engine at compile-time,
- The engine uses functions like `CreateResource<SomeScriptType>()`, which requires type information.

If the engine tries to **delete the scene pointer directly**, it results in undefined behavior due to the mismatch in heap ownership.

---

## The Solution

### DLL-Allocated, DLL-Deallocated Scenes

We define a clear rule:

> **Whoever allocates memory, deallocates it.**

The scene is:

- **Allocated inside the DLL**, because it contains DLL-defined types,
- **Destroyed via a DLL-exported `DestroyScene(Scene*)` function**, called from the engine during shutdown.

The engine still owns the lifecycle control: it knows *when* to destroy the scene. But the actual deletion is performed safely within the DLL’s memory context.

---

## Implementation

### DLL Side (Game Code)

```cpp
// Exported from the DLL
extern "C" __declspec(dllexport)
Scene* CreateScene() {
    return new Scene();
}

extern "C" __declspec(dllexport)
void DestroyScene(Scene* scene) {
    delete scene;
}
```

### Engine Side (EXE)

```cpp
cppCopyEdit// Called by the engine
Scene* scene = gameDll->CreateScene();

// ... run game loop ...

gameDll->DestroyScene(scene); // Proper cleanup
```

### Pros

- **Heap Safety**
   Prevents crashes and undefined behavior due to cross-heap deletion.
- **Encapsulation**
   Keeps internal logic and memory management inside the DLL.
- **Modular Design**
   The engine doesn't need to know or be rebuilt for every new script or type.
- **Clear Lifecycle Control**
   The engine still decides *when* cleanup happens.
- **Future-Proofing**
   The resource manager and scene internals can evolve without impacting the engine interface.

------

### Cons

- **DLL Interface Coupling**
   Requires clearly defined and maintained DLL exports (e.g., `DestroyScene()`).
- **Potential Misuse**
   A careless `delete scene;` in the EXE could cause a crash. Mitigated with smart pointers or wrappers.
- **More Boilerplate**
   Requires explicit destruction functions in DLL interface.

## Alternatives Considered

### EXE-Based Ownership

Allocating and destroying the scene in the EXE would require moving all script and component types into the engine. This approach was rejected because:

- Breaks plugin-based architecture,
- Requires engine recompilation for every new script,
- Prevents clean separation of game code and engine code.

### Shared Heap or Custom Allocator

Using a shared allocator between DLL and EXE is technically possible but unnecessarily complex for this problem. It adds extra overhead and debugging complexity, with minimal gain over simply letting the DLL clean up its own memory.



## Final Notes

This design reflects a balance between **safety**, **modularity**, and **extensibility**. It ensures:

- All user-defined types remain isolated in the DLL,
- The engine can manage application state and lifecycle,
- Cleanup is performed in the correct heap to avoid instability.

### Developer Guidelines

- Never call `delete` on objects created in the DLL from the EXE.
- Always use the exported `DestroyScene(Scene*)` or equivalent.
- Wrap external pointers in RAII handles if possible.
- Keep DLL interface versioned and minimal.