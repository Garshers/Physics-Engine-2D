# Physics Engine 2D

This project is a simple 2D physics engine implemented in C++. It uses OpenGL for rendering and GLFW for window and input management. The engine simulates basic physics interactions, including collisions and fluid dynamics.

## Features

- **Circle and Polygon Bodies**: Create and simulate circle and polygonal bodies.
- **Fluid Dynamics**: Simulate interactions with a body of water.
- **Collision Detection**: Efficient collision detection and response.
- **Rendering**: Render the bodies and fluids using OpenGL.
- **Camera Control**: Simple camera control with mouse and keyboard input.

## Prerequisites

- **C++ Compiler**: GCC, Clang, or MSVC.
- **OpenGL**: Graphics library for rendering.
- **GLFW**: Library for window and input management.
- **GLEW**: OpenGL Extension Wrangler Library.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Garshers/CarRentalWebsite.git
   cd CarRentalWebsite
   ```

2. **Install dependencies**:

Ensure you have OpenGL, GLFW, and GLEW installed on your system.

3. **Build the project**:
    ```bash
   g++ -o physics_engine main.cpp -lGL -lGLU -lglfw -lGLEW
   ```

## Usage

1. **Run the executable**:
    ```bash
   ./physics_engine
   ```

## Control the simulation:

- **Mouse**: Drag to move the camera.
- **Scroll**: Zoom in and out.
- **Arrow Keys**: Move selected bodies (if implemented).

## Multithreading
A dedicated thread handles intersection calculations for collision detection, ensuring smooth rendering and responsiveness.

## Code Overview

### Main Functions
- **CreateBoundaries**: Sets up the boundary walls for the simulation.
- **CreateBodies**: Creates random circle and polygon bodies within the simulation.
- **DrawCircle**: Renders a circle body.
- **DrawPolygon**: Renders a polygon body.
- **DrawLiquid**: Renders a body of water.
- **DrawBodies**: Renders all bodies and fluids in the simulation.
- **HandleArrowKeys**: Moves bodies using keyboard input.
- **cursor_pos_callback**: Handles mouse movement for camera control.
- **framebuffer_size_callback**: Adjusts the viewport on window resize.
- **scroll_callback**: Handles zooming with the mouse scroll.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License."# Physics-Engine-2D" 
