# Editor Overview

The **ConsoleGraphX Editor** is designed to enhance the development and debugging experience by providing visual tools that run alongside the game. The editor automatically spawns two key panels, along side the game application:

1. **Visual Logger**: Displays real-time log output for easier debugging and tracking of application events.
2. **Runtime Information Panel**: Shows live metrics and performance data of the running game.

These panels are displayed as separate console windows, which can be customized and moved independently to fit the developer's workflow, in side the editors main function.

## How Does It Work?

### Logger Integration

Typically, the logger outputs messages to a log file. However, when running within the editor, the logger switches to a special mode that redirects output to a **Visual Logger Window**. This is achieved through a method call in the logger that attaches a console window to the output stream.

The steps are as follows:

1. The logger initializes and sets up its default file-based output.
2. The editor detects if it is running and calls a specialized method in the logger to attach a visual console window.
3. The logger redirects its output to the visual window, allowing real-time log messages to be displayed.

This approach provides a seamless transition between standard logging and the enhanced visual logging experience in the editor.

### Runtime Information Panel

The **Runtime Information Panel** works similarly to the visual logger, but it is designed to provide live performance metrics and system information about the game. It displays data such as:

- Frames per second (FPS)
- Memory usage (WIP)
- Active scene and entity count
- CPU and GPU (WIP) usage (if applicable)

The panel is implemented using a separate window that is linked to the game engine’s performance tracking system. It receives updates in real-time and displays the information directly on the console.

The steps are as follows:

1. The editor initializes and creates a console window for the runtime information panel.
2. The engine’s performance tracking system attaches to this window and begins sending data updates.
3. The panel renders the received data in a human-readable format, allowing the developer to monitor performance metrics live.

### Multi-Window Design

The editor and the game run as separate processes. The editor manages the visual logger and runtime information panel as additional windows. This design provides the following benefits:

- **Separation of Concerns**: Each panel window is dedicated to a specific purpose (logging or performance metrics) and does not interfere with the game window.
- **Flexibility**: The panels can be moved, resized, or closed independently of the game window, allowing developers to customize their layout based on their needs.

### Summary

The ConsoleGraphX Editor enhances the development experience by:

- Providing a **Visual Logger** for real-time log output.
- Displaying a **Runtime Information Panel** with live performance metrics.
- Utilizing a **multi-window design** for flexibility and customization.

By integrating these features directly into the game engine, the editor offers a powerful, streamlined toolset for developers to debug and optimize their games more effectively.
