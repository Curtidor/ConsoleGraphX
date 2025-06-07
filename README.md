# ConsoleGraphX

**ConsoleGraphX** is a modular, high-performance game engine designed for building retro-style 2D games using the Windows console as a rendering backend. It provides modern engine architecture while embracing the constraints and aesthetics of classic terminal graphics.

---

##  Features

- **Console-Based Rendering**  
  Render directly to the Windows console using pixel-accurate buffer management — no external graphics APIs required.

-  **Modular Entity-Component System (ECS)**  
  Efficient runtime ECS with support for dynamic component creation, compile-time IDs for built-in types, and minimal memory overhead.

- **Scripting System**  
  Create and inject custom game logic with a flexible script component framework.

- **Sprite System**  
  Load, draw, and animate  `.cxsp` sprites with multiple layers and palette control. Includes `.cxanim` support for animation data (WIP).

- **Scene Management**  
  Seamlessly transition between multiple scenes, like levels or menus.

- **Multi-Console Windowing**  
  Spawn and manage multiple independent console windows (e.g. editor, runtime info panel, logger), each sharing memory buffers across processes.

-  **Built-in Logger & Profiler**  
  A real-time profiler displaying render times, sprite counts, and system metrics.

---

## Examples

### Editor View  

![Screenshot 2025-06-07 144411](https://github.com/user-attachments/assets/ddb2a5c5-2588-4594-9297-0ba0c66c4d62)

### Standalone Runtime  
![runtime](https://github.com/user-attachments/assets/65ae0ffe-dae5-46aa-9496-19f61166fb24)



## 📌 Project Structure

| Folder          | Description                                        |
| --------------- | -------------------------------------------------- |
| `ConsoleGraphX` | Core engine functionality and ECS                  |
| `Editor`        | Multi-window editor with game view and log panel   |
| `RunTime`       | Standalone game launcher                           |
| `WinCore`       | Windows-specific abstraction layer                 |
| `WindowHandler` | Handles spawning new console windows               |
| `External`      | External tools like TermLog (cross-process logger) |
| `Sandbox`       | Game dev/test playground                           |
| `Tools`         | Python-based tools for sprite and map editing      |



------

## Roadmap

- ✅ Multi-console window management
- ✅ Cross-process logging
- ✅ Editor with profiling and sprite rendering
- ✅ Premake build system (in progress)
- 🟡 In-editor tilemap editing
- 🟡 Asset packaging and export system
- 🟡 Scene live reload & debug tools
- 🟡 Sprite Animations (CURRENT FOCUS)
- 🟡 Physics (CURRENT FOCUS)

## Getting Started

### Setup Instructions

ConsoleGraphX uses **Premake5** to generate project files for Windows-based development environments. You can use Visual Studio, VS Code, or any other build tool that supports Windows C++ development.

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Curtidor/ConsoleGraphX.git
   ```

2. **Generate project files with Premake:**

   ```bash
   premake vs2022
   ```

   (Requires `premake5.exe` in your system PATH or placed in the project root)

3. **Open and build:**

   - Open `ConsoleGraphX.sln` in Visual Studio

   - Or use MSBuild or another Windows-compatible build system

4. **Run the engine:**
    Launch either the `Editor` or `RunTime` executable to start developing or testing your game.



## Conclusion and License

I invite you to explore **ConsoleGraphX** and leverage its powerful features to create engaging console-based 2D games (maybe even "3D"). Your contributions, feedback, and ideas are always welcome.

ConsoleGraphX is licensed under the **Mozilla Public License 2.0 (MPL-2.0)**.
 You are free to use, modify, and distribute the engine — including in commercial projects — provided that any modifications to engine source files are shared under the same license and original attribution is preserved.

If you have any questions or need further assistance, please don’t hesitate to reach out.

Enjoy your game development journey with ConsoleGraphX!

