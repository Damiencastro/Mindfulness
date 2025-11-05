# Build and Setup Guide
This document outlines the necessary components, libraries, and configuration required to build the Mindfulness application from source.

## Build System
    * CMake: A minimum version of `3.16` is required.
    * Compiler: A C++17 compatible compiler is necessary. The project is developed and tested with MinGW (GCC).
        * Crucial: The MinGW version used (e.g. 64-bit) must match the version of the pre-compiled Qt libraries being linked.

## Qt6 Dependencies
The project is built against Qt 6. The following components must be selected from the Qt Online Installer or Maintenance Tool:
    * Qt 6 (for Desktop Development):
        * MinGW [version] 64-bit
    * Modules:
        * Qt Core: For base application logic, QObject, signals/slots, and threading.
        * Qt Widgets: For all GUI elements (QApplication, QWidget, QDialog, QSystemTrayIcon, etc.)
        * Qt Test: (Optional) Required for building and running unit tests.

## Windows Libraries
The application links against several core Windows libraries for its monitoring and process-management capabilities. These are included with the Windows SDK (which comes with MinGW or Visual Studio Build Tools).
The following libraries must be linked in `CMakeLists.txt`:
    * `Psapi.lib` (Process Status API): Used by `ProcessUtils` to enumerate processes (`EnumProcesses`) and get process names (`GetModuleBaseNameW`).
    * `User32.lib`: Used for core windowing functionality.

### Example CMake linkage:

        target_link_libraries(Mindfulness PRIVATE
            Qt6::Widgets
            Psapi
            User32
        )


## Asset Bundling (`resources.qrc`)
Application icons (specifically the system tray icon) are bundled directly into the executable using the Qt Resource System.

1. Create the .qrc file: A file named resources.qrc is included in the project root. This XML file defines aliases for asset files.
```XML
    <!DOCTYPE RCC>
    <RCC version="1.0">
    <qresource prefix="/">
        <file alias="icons/app.ico">icons/app_icon.png</file>
    </qresource>
    </RCC>
```

2. Add to CMake: The resources.qrc file must be added to the add_executable() list in CMakeLists.txt.

3. Enable AUTORCC: The CMAKE_AUTORCC variable must be set to ON in CMakeLists.txt to ensure CMake automatically processes this file during the build.
```cmake
    set(CMAKE_AUTORCC ON)

    add_executable(Mindfulness
        main.cpp
        # ... all other .cpp and .h files ...
        resources.qrc
    )
```