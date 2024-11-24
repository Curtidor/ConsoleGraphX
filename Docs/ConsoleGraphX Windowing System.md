# ConsoleGraphX Windowing System

### !!!!!(NOT UP TO DATE)!!!!!

## Overview

The ConsoleGraphX windowing system is designed to support multiple windows within a console-based game engine. This enables the creation of auxiliary windows (such as logger windows, debug panels, or mini-maps) alongside the main game window. Each window can be managed independently, with some running in separate processes to handle rendering or logging tasks, while others run within the main process.

## Design Components

The window system is composed of several main classes:

- **Window**: Represents a console window with rendering and event handling capabilities. It is the top-level class responsible for managing the console window, position, and size, and is capable of operating in both in-process and cross-process modes.
- **Screen**: Manages a logical screen within the window, responsible for higher-level rendering functions such as drawing text and setting colors.
- **PixelCanvas**: Provides general-purpose pixel manipulation methods and acts as an interface to the underlying buffer.
- **PixelBuffer**: Stores the raw pixel data (`CHAR_INFO`) used for rendering, with two variants for handling in-process and cross-process scenarios.

## Class Breakdown

---

### 1. `Window`

The `Window` class represents a console-based window and manages its creation, destruction, and event handling. It supports two modes:

1. **In-process windows**: These windows are created within the same process as the main application and are capable of rendering directly to the console.
2. **Cross-process windows**: These windows are created as separate processes, with shared memory handling the communication between processes. The main process writes to the shared memory, while the child process handles rendering.

#### Key Features

- **Dual Constructors**:
  - `Window(width, height, windowName, sBuffer, fontWidth, fontHeight)`: Used for cross-process windows, where a pre-existing `PixelBuffer` is passed in.
  - `Window(width, height, windowName, fontWidth, fontHeight)`: Used for in-process windows, where the window creates and manages its own buffer.
  
- **EmbeddedWindow**: A derived class specifically for creating an in-process window within the main process’s console. This is useful for the main game window, which doesn't require a separate console process.

#### Methods

- **`Destroy()`**: Cleans up resources manually and invokes `OnWindowDestroyed` to notify other components. This explicit destroy method is used instead of a destructor to ensure proper cleanup and event notification before deletion.
- **`CreateConsoleWindow()`**: Initializes a new console window by creating a process and accessing shared memory. Cross-process windows use `_CreateWindowImpl` to spawn a new console process, while in-process windows are handled differently in `EmbeddedWindow`.
- **`SetWindowPosition(x, y)`**: Sets the position of the window.
- **`GetWindowPosition()`**: Retrieves the current position and size of the window as `WindowPositionData`.

#### Example Usage

```cpp
// Creating a window in-process
EmbeddedWindow mainWindow(300, 120, "Game Window", 1, 1); 

// Creating a cross-process window
std::unique_ptr<PixelBuffer> sharedBuffer = /* obtain shared buffer */;
Window crossProcessWindow(300, 120, "Logger", std::move(sharedBuffer), 1, 1);

```
## 2. Screen

The `Screen` class represents a logical screen within a window. It inherits from `PixelCanvas` and handles high-level rendering actions, such as drawing characters and text, setting pixel colors, and managing screen properties.

### Key Functions

#### Constructors

- **`Screen(width, height, fontWidth, fontHeight, sBuffer)`**: Used for cross-process windows where a shared `PixelBuffer` is passed.
- **`Screen(width, height, fontWidth, fontHeight)`**: Used for in-process windows where a `PixelBufferHandle` is created internally.

#### Render and Manipulation Methods

- **`DrawScreen()`**: Renders the buffer to the console.
- **`WriteText(text, x, y)`**: Writes text at the specified coordinates.
- **`SetPalletColors_A(paletteColors)`**: Manages palette colors for console output.

The `Screen` class provides a high-level interface for managing the visual contents of the window, allowing both in-process and cross-process windows to use the same rendering logic while accessing different types of `PixelBuffer`.

---

## 3. PixelCanvas

`PixelCanvas` provides a generalized interface for interacting with pixel data. It doesn’t handle console rendering directly; instead, it offers methods for manipulating the underlying pixel buffer, which `Screen` or other derived classes can then use to render.

### Key Functions

#### Constructors

- **`PixelCanvas(width, height)`**: Creates a canvas with an in-process buffer (`PixelBufferHandle`).
- **`PixelCanvas(width, height, sBuffer)`**: Uses an existing buffer, intended for cross-process use.

#### Pixel Manipulation

- **`SetPixel(x, y, s_pixel)`**: Sets a single pixel at the specified coordinates.
- **`FillCanvas(fillChar)`**: Fills the entire canvas with a specific character.

`PixelCanvas` enables the `Screen` class to work with both in-process and cross-process buffers without needing to know the specifics of buffer management.

---

## 4. PixelBuffer

`PixelBuffer` is the lowest-level component, directly managing the `CHAR_INFO` array that holds the pixel data. This class has two versions:

- **`PixelBuffer`**: Basic buffer for cross-process usage. This version does not have a handle because it doesn’t need direct access to the console.
- **`PixelBufferHandle`**: A buffer with a console handle for in-process rendering.

### Key Functions

#### Constructors

- **`PixelBuffer(width, height)`**: General-purpose buffer.
- **`PixelBufferHandle(handle, width, height)`**: In-process buffer with console handle.

#### Data Access

- **`GetBuffer()`**: Returns the `CHAR_INFO` array.
- **`GetConsoleHandle()`**: Returns the console handle (only available in `PixelBufferHandle`).

This separation allows the system to handle different rendering needs based on where the buffer resides, providing flexibility and avoiding unnecessary handle allocation for cross-process buffers.

---

## Why This Structure?

### Cross-Process Rendering

The dual approach to process management allows ConsoleGraphX to support multiple windows effectively:

- **Main Process Windows** (like the main game window): These interact directly with the console through `PixelBufferHandle`, allowing real-time updates and fast rendering.
- **Child Process Windows** (like logging windows): These use shared memory with `PixelBuffer`. In this setup, the child process reads from shared memory and handles rendering, while the main process writes to this memory.

### Modularity and Flexibility

- **Screen and Canvas Separation**: Separating `Screen` from `PixelCanvas` provides a clear boundary between drawing logic and buffer management, enabling code reuse for both in-process and cross-process windows.
- **PixelBuffer Variants**: The two `PixelBuffer` classes enable the system to optimize buffer handling for in-process and cross-process scenarios, allowing the main process to share pixel data without requiring direct console access.

--- 

This structure allows ConsoleGraphX to handle multiple windows in a flexible, modular way, supporting both in-process and cross-process rendering needs. Each layer in the hierarchy—`Window`, `Screen`, `PixelCanvas`, and `PixelBuffer`—has a distinct responsibility, ensuring clean separation of concerns and making the system scalable for complex applications.
