# ConsoleGraphX Style Guide

## 1. Directory and File Naming Conventions

- **Directories:** Use `PascalCase` for all directory names.
  - Example: `ScreenGraphics`, `Buffer`, `Graphics`

- **File Names:** Use `snake_case` for all file names except for specific cases mentioned below.
  - Example: `pixel_buffer.cpp`, `palette_manager.cpp`

- **Special Files:**
  - **Precompiled Headers:** Use `PCH_FILENAME.h` in uppercase.
    - Example: `PCH_CGX.h`
  - **Library Headers and Implementation Files:** Use `PascalCase` to improve readability.
    - Example: `WinCore.h`, `GraphicsUtil.h`, `WinCore.cpp`

## 2. Class Naming Conventions

- **Class Names:** Use `PascalCase` for all class names.
  - Example: `PixelBuffer`, `ScreenCanvas`

## 3. Function and Method Naming Conventions

- **Function/Method Names:** Use `PascalCase` for all function and method names.
  - Example: `InitializeBuffer()`, `RenderFrame()`

## 4. Variable Naming Conventions

- **Private/Protected Members:** Use `_m_` prefix followed by `camelCase`.
  - Example: `_m_bufferSize`, `_m_pixelArray`

- **Public Members:** Use `m_` prefix followed by `camelCase`.
  - Example: `m_width`, `m_height`

- **Local Variables and Function Parameters:** Use `camelCase`.
  - Example: `bufferSize`, `pixelIndex`

## 5. Macro Naming Conventions

- **Macros:** Use project initials as a prefix (`CGX_` for ConsoleGraphX) followed by the macro name in `UPPER_SNAKE_CASE`.
  - Example: `CGX_BUFFER_SIZE`, `CGX_RENDER_MODE`

## 6. Namespace Naming Conventions
- **Namespaces** Use `PascalCase`, for nested or internal namespaces, separate with an underscore `_`.
  - Example: `ConsoleGraphX`, `ConsoleGraphX_Internal`

## 7. Constants

- **Global and Class Constants:** Use `UPPER_SNAKE_CASE` for constants.
  - Example: `MAX_BUFFER_SIZE`, `DEFAULT_COLOR`

## 8. Exception Handling and Error Codes

- **Error Codes:** Use `PascalCase` for error enums or error classes to clearly distinguish them.
  - Example: `ErrorCode`, `RenderError`

## 9. Special Case Suggestions

- **Type Definitions (Typedefs or Aliases):** Use `PascalCase` for type aliases.
  - Example: `using ColorType = uint32_t;`

- **Enumerations:** Use `PascalCase` for enums and their values, with values also in `PascalCase`.
  - Example:
    ```cpp
    enum class RenderMode {
        FullScreen,
        Windowed,
        Borderless
    };
    ```

## 10. File Organization and Headers

- **Header Guards:** Use `#pragma once` at the beginning of all header files to prevent multiple inclusions.

- **Include Order:** Use the following order to improve readability:
  1. C++ standard library headers.
  2. Third-party library headers.
  3. Related header
  4. Other project headers.

- **Commenting Style:** Use `Doxygen` style comments for all functions, classes, and important variables.
  ```cpp
  /**
   * @brief Brief description of the function.
   * @param parameter_name Description of the parameter.
   * @return Description of the return value.
   */
**NOTE (temp)** Moving forward all documentation must be ported to use this format.
