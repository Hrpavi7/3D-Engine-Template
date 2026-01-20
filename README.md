# 3D Engine Template

This is a 3D Engine Template that you can use.

## Table of Contents

1. [Installation](#installation)
2. [Usage](#usage)
3. [Technical Details](#technical-details)
4. [Controls](#controls)
5. [Building from Source](#building-from-source)
6. [Screenshot](#screenshot)
7. [Project Structure](#project-structure)
8. [Contributing](#contributing)
9. [License](#license)

## Installation

### Prerequisites
- Windows system with OpenGL 3.3 compatible graphics card
- MinGW-w64 compiler (for building from source)
- Pre-built executable available in `build/minecraft.exe`

### Quick Start
1. Navigate to the `build/` directory
2. Double-click `minecraft.exe` to run the game
3. The game will open in a window with a flat voxel world

## Usage

The game features a procedurally generated flat world with different block types (grass, dirt, stone). Players can move around the world using first-person controls and explore the voxel environment.

### Key Features
- Real-time 3D rendering with OpenGL 3.3
- First-person camera with mouse look
- Procedural world generation
- Block-based terrain with multiple textures
- Smooth movement and collision detection

## Technical Details

### Rendering Engine
- **OpenGL Version**: 3.3 core profile
- **Shader Pipeline**: Custom vertex and fragment shaders
- **Vertex Format**: Position (3 floats), UV (2 floats), Normal (3 floats)
- **Texture Atlas**: 64x16 pixel atlas with 16x16 tiles

### Camera System
- First-person perspective with yaw/pitch rotation
- Forward and right vector calculations for relative movement
- Smooth mouse look with configurable sensitivity
- Collision detection with world boundaries

### World Generation
- Flat world with grass, dirt, and stone layers
- Procedural block placement
- Efficient mesh generation and rendering
- Dynamic vertex buffer management

### Performance
- Optimized mesh building with dynamic arrays
- Efficient rendering with indexed vertex buffers
- Minimal memory footprint for voxel data
- 60+ FPS on modern hardware

## Controls

### Movement
- **W**: Move forward (relative to camera view)
- **S**: Move backward (relative to camera view)
- **A**: Move left (strafe, relative to camera view)
- **D**: Move right (strafe, relative to camera view)
- **Shift**: Sprint (increased movement speed)
- **Space**: Jump (upward velocity)

### Camera
- **Mouse**: Look around (yaw and pitch rotation)
- **Mouse Sensitivity**: 0.0022f for smooth control
- **First-person view**: Camera positioned at player eye level

### System
- **Escape**: Exit the game
- **Window**: 800x600 resolution (configurable in source)

## Building from Source

### Prerequisites
- MinGW-w64 compiler (tested with D:/Development/MinGW)
- GCC with C11 support
- Windows SDK with OpenGL libraries
- Basic knowledge of C programming

### Build Commands

#### Using GCC Directly (Recommended)
```bash
gcc -std=c11 -O2 -Wall -Wextra -Isrc\include src\*.c -o build\minecraft.exe -lopengl32 -lgdi32 -luser32 -lkernel32 -lwinmm
```

#### Using Makefile (if make is available)
```bash
make all     # Build the executable
make run     # Build and run the game
make clean   # Clean build artifacts
```

### Build Configuration
- **Compiler**: GCC with C11 standard
- **Optimization**: -O2 for release builds
- **Warnings**: -Wall -Wextra for code quality
- **Libraries**: OpenGL32, GDI32, User32, Kernel32, WinMM

## Screenshot

![Minecraft C Voxel Game Screenshot](screenshot.png)

### Taking Screenshots
To add your own screenshot:
1. Run the game and position the camera
2. Take a screenshot using your preferred method
3. Save it as `screenshot.png` in the project root
4. Update the README.md file if needed

## Project Structure

```
d:\MyFolders\development\cc++\games\minecraft\
├── build/
│   └── minecraft.exe          # Pre-built executable
├── src/
│   ├── include/               # Header files
│   │   ├── app.h             # Application interface
│   │   ├── camera.h          # Camera system
│   │   ├── gl_loader.h       # OpenGL loading
│   │   ├── math4.h           # Math utilities
│   │   ├── mesh.h            # Mesh structures
│   │   ├── renderer.h        # Rendering system
│   │   └── world.h           # World generation
│   ├── app_win32.c           # Windows application layer
│   ├── camera.c              # Camera implementation
│   ├── gl_loader.c           # OpenGL function loading
│   ├── main.c                # Main game loop
│   ├── math4.c               # Math library
│   ├── mesh.c                # Mesh management
│   ├── renderer.c            # OpenGL rendering
│   └── world.c               # World generation
├── Makefile                   # Build configuration
└── README.md                  # This file
```

### Key Source Files
- **[main.c](src/main.c)**: Core game loop, input handling, mesh generation
- **[camera.c](src/camera.c)**: Camera movement, vector calculations, collision
- **[renderer.c](src/renderer.c)**: OpenGL initialization, shader compilation, mesh rendering
- **[world.c](src/world.c)**: World generation, block placement, mesh building
- **[mesh.c](src/mesh.c)**: Memory management for vertex data

## Contributing

### Development Guidelines
1. Fork the repository and create a feature branch
2. Follow existing code style and conventions
3. Test changes thoroughly before submitting
4. Update documentation for new features
5. Submit pull requests with detailed descriptions

### Areas for Improvement
- **Block Interaction**: Add block breaking/placing mechanics
- **Inventory System**: Implement item management
- **Biome Generation**: Create diverse terrain types
- **Lighting**: Add dynamic lighting and shadows
- **Sound Effects**: Implement audio system
- **Multiplayer**: Add network support for multiple players

### Code Quality
- Maintain consistent indentation and formatting
- Use meaningful variable and function names
- Add error handling for edge cases
- Optimize performance-critical code
- Document complex algorithms and math

## License

This project is open source and available under the MIT License. Feel free to use, modify, and distribute the code according to the license terms.

### Attribution
- Built with OpenGL and MinGW-w64
- Inspired by Minecraft's voxel-based gameplay
- Educational project for learning 3D graphics programming

---

*Last updated: January 2026*

*Version: 1.0*
