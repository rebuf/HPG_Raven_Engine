# Raven Engine

The Game Engine was developed by the Raven group at the University of Leeds:
 - Ammar Herzallah (Rendering, Terrain, Foliage)
 - Ben Husle 
 - Thomas Moreno Cooper 
 - Sulagna Sinha 
 - Tian Zeng

for the High-Performance Graphics and Games Engineering group project module. [The Original Repository.](https://gitlab.com/SC17BH/hpg_group_project)

The Engine developed using c++. The Renderer was made completely from scratch using OpenGL 4.5, the renderer only uses an opengl extension loader library GLEW. The renderer is designed with performance and extensibility in mind, it performes culling, render batching based on shader and materials, instancing for terrain foliage and more. The renderer have miltiple passess Deferred(opaque & masked), and Forward(translucent).


## Features
The Engine have so many features this is a snapshot of them

![Screenshot (15)](https://user-images.githubusercontent.com/56574326/119246934-9597ed00-bb7d-11eb-8406-562df6b51526.png)

### 1. Physically Based Rendering & Image Based Lighting.
Compute lighting using a physically based model similar to UE4. Using Real Shading in Unreal Engine 4, by Brian Karis, Epic Games as a reference for our lighting model. pre-filter a spherical images or the current captured sky for IBL to provide beautiful reflection and indirect lighting.


### 2. Advance Materail System & Custom Shaders.
Provide the ability to create materials with custom shader that override a function in the fragment and vertex shader to add advanced techniques. Whether you want to do simple UV scaling or grass wind animation you can easily do so using the material system.

### 3. SSAO
<br>

![Editor_Sky_Anim](https://user-images.githubusercontent.com/56574326/119244032-3d082600-bb64-11eb-9da2-635607da0a4f.png)
### 4. Skeletal Animation
Play skeletal animation on skinned meshes and build animation state machine using the animation node editor.

### 5. Sun Shadow with cascading shadow maps.
Sun shadow with up to 4 cascading shadow maps. Useful for large terrain scenes where you want to have shadows viewed from very far distances and still maintain high resolution for closer objects.

### 6. Physically Based Sky.
Compute the sky using a physically based module that approximate Rayleigh and Mie scattering. The approch used was provided by Sean O'Neil, Chapter 16. Accurate Atmospheric Scattering, GPU Gems 2. The approach used is compute everything at run time inside one shader without any pre-calculation. The sky is also captured and filtered for IBL whenever its edited or updated, this provides an environment light source that reflects the current sky.
<br><br>

![TES_2_c](https://user-images.githubusercontent.com/56574326/119245319-5020f300-bb70-11eb-8620-6286dac25f0b.png)
### 7. Procedural and Tessellated Terrain.
Generate terrain procedurally and fill it with grass and trees using our foliage system. The terrain uses TCS & TES shaders provided by OpenGL 4.*, the tessellation levels of the terrain mesh is computed based on its size in screen coordinate.
<br>

![Foliage_ _Sky](https://user-images.githubusercontent.com/56574326/119244055-7e003a80-bb64-11eb-9e8d-e8fd104698d6.png)
### 8. Instanced Foliage System.
A system for adding foliage layers on top of the terrain, each layer will be rendered using OpenGL instanced rendering in a single draw call. The system also provide culling based on terrain bins and view distance.





# Build

the solution files will be generated automaticlly by premake

1. Run GenerateVS19-x64.bat or GenerateVS19-Win32.bat
2. Go to build folder and open Rave.sln
3. the bin,bin-int,build folders should be ignored when you push your code. I add them into the .gitignore


