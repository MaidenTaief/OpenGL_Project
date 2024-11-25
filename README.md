## **Table of Contents**

1. **Introduction**
2. **Project Overview**
3. **Code Structure and Sequence**
4. **Detailed Explanation of Each Component**
   - 4.1. `main.cpp`
   - 4.2. `Camera.h` and `Camera.cpp`
   - 4.3. `Window.h` and `Window.cpp`
   - 4.4. `Shader.h` and `Shader.cpp`
   - 4.5. `Terrain.h` and `Terrain.cpp`
   - 4.6. `HikingData.h` and `HikingData.cpp`
   - 4.7. `HikingVisualizer.h` and `HikingVisualizer.cpp`
   - 4.8. `Skybox.h` and `Skybox.cpp`
5. **Shaders**
   - 5.1. `vertex_shader.glsl`
   - 5.2. `fragment_shader.glsl`
   - 5.3. `skybox_vertex.glsl`
   - 5.4. `skybox_fragment.glsl`
6. **Resource Files**
   - 6.1. Heightmap and Texture Images
   - 6.2. Skybox Textures
   - 6.3. GPX Data File
7. **Execution Flow**
8. **Conclusion**

---

## **1. Introduction**

This documentation provides a detailed explanation of the OpenGL Hiking Simulator project, which visualizes a hiking trail over a 3D terrain with a skybox background. The simulator reads hiking data from a GPX file, generates a terrain from a heightmap image, and renders the trail and terrain using OpenGL.

---

## **2. Project Overview**

The project consists of several interconnected components:

- **Main Application (`main.cpp`)**: Initializes the application, handles the main loop, and coordinates the rendering process.
- **Camera System**: Manages the camera's position and orientation in the 3D scene.
- **Window Management**: Handles window creation and input callbacks.
- **Shader Management**: Loads and compiles shader programs used for rendering.
- **Terrain Generation**: Creates a 3D terrain mesh from a heightmap image.
- **Hiking Data Processing**: Loads and processes hiking trail data from a GPX file.
- **Hiking Visualizer**: Animates the hiker along the trail and renders the trail.
- **Skybox Rendering**: Provides a surrounding environment using a skybox.

---

## **3. Code Structure and Sequence**

Below is the sequence of code files and how they are connected:

1. **`main.cpp`**: Entry point of the application.
2. **`Window.h` / `Window.cpp`**: Handles window initialization and input callbacks.
3. **`Camera.h` / `Camera.cpp`**: Manages camera movement and view matrix calculations.
4. **`Shader.h` / `Shader.cpp`**: Loads, compiles, and manages shader programs.
5. **`Terrain.h` / `Terrain.cpp`**: Generates and renders the terrain mesh.
6. **`HikingData.h` / `HikingData.cpp`**: Loads and processes hiking trail data from a GPX file.
7. **`HikingVisualizer.h` / `HikingVisualizer.cpp`**: Animates the hiker along the trail and renders the trail.
8. **`Skybox.h` / `Skybox.cpp`**: Renders the skybox surrounding the scene.
9. **Shaders**: GLSL shader files used by the rendering components.

---

## **4. Detailed Explanation of Each Component**

### **4.1. `main.cpp`**

**Purpose**: Serves as the entry point of the application. Initializes all components, handles the main render loop, and coordinates the rendering of the scene.

**Key Responsibilities**:

- Initialize GLFW and GLEW.
- Set up the window and input callbacks.
- Load resources (terrain, hiking data, skybox).
- Initialize the camera.
- Handle the main rendering loop.
- Update the hiker's position.
- Render the terrain, hiking trail, and skybox.
- Clean up resources upon exit.

**Connection to Other Components**:

- Utilizes `Window` for window management.
- Uses `Camera` for view transformations.
- Loads shaders via the `Shader` class.
- Initializes and renders `Terrain`.
- Loads hiking data using `HikingData`.
- Animates and renders the hiker using `HikingVisualizer`.
- Renders the skybox using `Skybox`.

**Main Flow in `main.cpp`**:

1. **Initialization**:
   - Create a `Window` instance.
   - Initialize GLFW and GLEW.
   - Set input callbacks.
   - Enable OpenGL depth testing.

2. **Resource Loading**:
   - Load hiking data from a GPX file using `HikingData`.
   - Initialize the `HikingVisualizer` with the hiking data.
   - Initialize the `Terrain` with the heightmap and texture.
   - Initialize the `Skybox` with skybox textures.

3. **Main Loop**:
   - Calculate `deltaTime` for animations.
   - Process input (keyboard and mouse).
   - Update the hiker's position in `HikingVisualizer`.
   - Clear the screen and set up the view and projection matrices.
   - Render the `Terrain`.
   - Render the hiking trail and hiker using `HikingVisualizer`.
   - Render the `Skybox`.
   - Swap buffers and poll events.

4. **Cleanup**:
   - Delete shader programs.
   - Terminate GLFW.

### **4.2. `Camera.h` and `Camera.cpp`**

**Purpose**: Manages the camera's position, orientation, and generates the view matrix used in rendering.

**Key Responsibilities**:

- Handle camera movement based on user input.
- Calculate the view matrix for the current camera position and orientation.
- Provide methods to set and retrieve camera parameters.

**Connection to Other Components**:

- Used in `main.cpp` to obtain the view matrix for rendering.
- Input callbacks in `main.cpp` modify the camera's position and orientation.

**Main Functions**:

- `getViewMatrix()`: Returns the current view matrix.
- Movement functions (e.g., `ProcessKeyboard`, `ProcessMouseMovement`): Update the camera's position and orientation.

### **4.3. `Window.h` and `Window.cpp`**

**Purpose**: Handles the creation and management of the GLFW window and sets up input callbacks.

**Key Responsibilities**:

- Initialize GLFW and create a window context.
- Set up input modes and callbacks for keyboard and mouse input.
- Provide access to the GLFW window instance.

**Connection to Other Components**:

- `main.cpp` uses `Window` to create the application window.
- Input callbacks are set up in `main.cpp` but utilize the window instance from `Window`.

### **4.4. `Shader.h` and `Shader.cpp`**

**Purpose**: Loads, compiles, and manages shader programs used for rendering different components.

**Key Responsibilities**:

- Read shader source code from files.
- Compile vertex and fragment shaders.
- Link shaders into a shader program.
- Provide methods to set uniform variables in shaders.

**Connection to Other Components**:

- Used by `Terrain`, `HikingVisualizer`, and `Skybox` to manage their respective shaders.
- Provides a common interface for shader operations.

**Main Functions**:

- `load(vertexPath, fragmentPath)`: Loads and compiles the shaders from files.
- `use()`: Activates the shader program.
- Uniform setter methods (e.g., `setMat4`, `setInt`): Set shader uniform variables.

### **4.5. `Terrain.h` and `Terrain.cpp`**

**Purpose**: Generates a 3D terrain mesh from a heightmap image and renders it.

**Key Responsibilities**:

- Load a heightmap image and create a heightmap array.
- Generate vertices and indices for the terrain mesh.
- Calculate normals for lighting.
- Load a texture to apply to the terrain.
- Set up OpenGL buffers (VAO, VBO, EBO).
- Render the terrain mesh.

**Connection to Other Components**:

- Initialized and used in `main.cpp`.
- Uses `Shader` to manage the terrain shader program.
- Shares scaling factors with `HikingData` to ensure alignment with the hiking trail.

**Main Flow in `Terrain`**:

1. **Initialization**:
   - Load the heightmap image into an array.
   - Generate the terrain mesh (vertices and indices).
   - Calculate normals for lighting.
   - Load the terrain texture.
   - Set up OpenGL buffers.

2. **Rendering**:
   - In the `draw` method, set shader uniforms (model, view, projection matrices).
   - Bind the texture and VAO.
   - Issue the draw call to render the terrain.

**Important Methods**:

- `initialize(heightMapPath, texturePath, hikingData)`: Initializes the terrain with the given resources.
- `generateTerrainMesh(heightData, width, height)`: Creates the terrain mesh.
- `draw(view, projection)`: Renders the terrain.

### **4.6. `HikingData.h` and `HikingData.cpp`**

**Purpose**: Loads and processes hiking trail data from a GPX file.

**Key Responsibilities**:

- Parse the GPX file to extract GPS coordinates and elevation data.
- Convert GPS coordinates to local coordinates matching the terrain.
- Store the hiking trail points for use in visualization.

**Connection to Other Components**:

- Used in `main.cpp` to load hiking data.
- Provides the trail points to `HikingVisualizer`.
- Shares scaling factors with `Terrain` to ensure alignment.

**Main Functions**:

- `loadHikingData(filename, hikingPoints)`: Loads and processes the GPX file.
- `gpsToLocalCoordinates(lat, lon, ele)`: Converts GPS coordinates to local coordinates.

**Processing Steps**:

1. **Load GPX File**:
   - Use `tinyxml2` library to parse the GPX file.
   - Extract track points (`<trkpt>` elements) with latitude, longitude, and elevation.

2. **Calculate Min/Max Latitude and Longitude**:
   - Determine the minimum latitude and longitude to center the trail.

3. **Convert Coordinates**:
   - Convert GPS coordinates to local coordinates using scaling factors.
   - Center the coordinates relative to the terrain.

### **4.7. `HikingVisualizer.h` and `HikingVisualizer.cpp`**

**Purpose**: Animates the hiker along the trail and renders the hiking trail.

**Key Responsibilities**:

- Store and manage the trail points.
- Update the hiker's position along the trail based on time and speed.
- Render the trail as a line strip.
- Render the hiker's current position as a point.

**Connection to Other Components**:

- Initialized in `main.cpp` with the hiking data.
- Uses `Shader` to manage the trail shader program.
- Shares scaling factors with `Terrain` and `HikingData` for alignment.

**Main Flow in `HikingVisualizer`**:

1. **Initialization**:
   - Load shaders for rendering the trail and hiker.
   - Set up OpenGL buffers for the trail and hiker position.
   - Store the trail points.

2. **Update**:
   - Calculate the hiker's new position along the trail based on speed and `deltaTime`.
   - Update the hiker's position in the OpenGL buffer.

3. **Rendering**:
   - In the `draw` method, set shader uniforms (view and projection matrices).
   - Render the trail using a line strip.
   - Render the hiker's current position as a point.

**Important Methods**:

- `initialize(hikingPoints)`: Sets up the visualizer with the trail data.
- `update(deltaTime)`: Updates the hiker's position.
- `draw(view, projection)`: Renders the trail and hiker.
- `getCurrentHikerPosition()`: Returns the hiker's current position for camera following.

### **4.8. `Skybox.h` and `Skybox.cpp`**

**Purpose**: Renders the skybox, providing a surrounding environment for the scene.

**Key Responsibilities**:

- Load skybox textures.
- Set up a cube mesh for the skybox.
- Render the skybox with appropriate depth testing.

**Connection to Other Components**:

- Initialized in `main.cpp` with the skybox textures.
- Uses `Shader` to manage the skybox shader program.
- Rendered last in the main render loop to ensure proper depth handling.

**Main Flow in `Skybox`**:

1. **Initialization**:
   - Load the skybox textures into a cubemap texture.
   - Set up the cube mesh vertices and OpenGL buffers.
   - Load shaders for rendering the skybox.

2. **Rendering**:
   - Adjust depth testing to render the skybox correctly.
   - Remove the translation component from the view matrix.
   - Set shader uniforms (view and projection matrices).
   - Bind the cubemap texture and VAO.
   - Render the skybox cube.

**Important Methods**:

- `initialize(faces)`: Loads the skybox textures and sets up buffers.
- `draw(view, projection)`: Renders the skybox.

---

## **5. Shaders**

Shaders are small programs that run on the GPU to control the rendering pipeline. They are written in GLSL.

### **5.1. `vertex_shader.glsl`**

**Used By**: `Terrain`

**Purpose**: Processes each vertex of the terrain mesh, applying transformations and passing data to the fragment shader.

**Key Responsibilities**:

- Transform vertex positions to clip space using the model, view, and projection matrices.
- Pass normals and texture coordinates to the fragment shader.

**Key Variables**:

- Inputs: `aPos`, `aNormal`, `aTexCoords`.
- Outputs: `FragPos`, `Normal`, `TexCoords`.
- Uniforms: `model`, `view`, `projection`.

### **5.2. `fragment_shader.glsl`**

**Used By**: `Terrain`

**Purpose**: Calculates the color of each fragment (pixel) of the terrain, applying lighting and texturing.

**Key Responsibilities**:

- Sample the terrain texture using texture coordinates.
- Calculate lighting based on normals and light direction.
- Output the final color.

**Key Variables**:

- Inputs: `FragPos`, `Normal`, `TexCoords`.
- Outputs: `FragColor`.
- Uniforms: `terrainTexture`.

### **5.3. `skybox_vertex.glsl`**

**Used By**: `Skybox`

**Purpose**: Processes each vertex of the skybox cube.

**Key Responsibilities**:

- Transform vertex positions to clip space.
- Pass texture coordinates to the fragment shader.

**Key Variables**:

- Input: `aPos`.
- Output: `TexCoords`.
- Uniforms: `view`, `projection`.

### **5.4. `skybox_fragment.glsl`**

**Used By**: `Skybox`

**Purpose**: Samples the cubemap texture to color each fragment of the skybox.

**Key Responsibilities**:

- Sample the cubemap texture using the texture coordinates.
- Output the final color.

**Key Variables**:

- Input: `TexCoords`.
- Output: `FragColor`.
- Uniforms: `skybox`.

---

## **6. Resource Files**

### **6.1. Heightmap and Texture Images**

- **Heightmap (`hoydedata_svarthvitt.png`)**: A grayscale image where the intensity represents elevation.
- **Terrain Texture (`tex2.png`)**: An image applied to the terrain surface to give it color and texture.

### **6.2. Skybox Textures**

- A set of six images representing the faces of a cube (right, left, top, bottom, front, back).
- High-resolution images (e.g., 2048x2048 pixels) to provide a detailed background.

### **6.3. GPX Data File**

- **GPX File (`Afternoon_Run.gpx`)**: Contains GPS track data with latitude, longitude, and elevation points.
- Used to generate the hiking trail.

---

## **7. Execution Flow**

1. **Program Start**:
   - `main.cpp` begins execution.
   - Initializes window, OpenGL context, and sets up callbacks.

2. **Resource Loading**:
   - Hiking data is loaded from the GPX file using `HikingData`.
   - The `HikingVisualizer` is initialized with the hiking data.
   - The `Terrain` is initialized with the heightmap and texture.
   - The `Skybox` is initialized with skybox textures.

3. **Camera Setup**:
   - The camera is positioned appropriately to view the terrain and trail.

4. **Main Render Loop**:
   - **Input Processing**: Handle user input for camera movement and interactions.
   - **Updates**:
     - `HikingVisualizer` updates the hiker's position.
   - **Rendering**:
     - Clear the screen.
     - Calculate the view and projection matrices.
     - Render the `Terrain`.
     - Render the hiking trail and hiker using `HikingVisualizer`.
     - Render the `Skybox`.
   - **Buffer Swap**: Swap buffers to display the rendered frame.
   - **Event Polling**: Poll for and process events.

5. **Program Termination**:
   - Clean up resources.
   - Terminate GLFW and exit.

---

## **8. Conclusion**
