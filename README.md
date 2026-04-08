Fractal Generator is a program dedicated to the visualization of Julia fractals in C++ using OpenGL.
The application displays beautiful Julia fractals in real time. The selection of fractals is random, although an algorithm ensures that only visually interesting ones are chosen.
The selection and generation of fractals are highly optimized, using multithreading and GPU computation via OpenGL.
The program offers dynamic animation of Julia fractals by interpolating them using Bézier curves.

![Fractal Generator](https://github.com/user-attachments/assets/3d70dc13-39cd-4115-ad18-476f0f5a8939)

The projet is compatible with OpenGL 4.6 and OpenGL ES 3.2 device.

Installation process : 

Using OpenGL 4.6 (recommended):

Linux :

Clone the Linux branch

Then make sure all dependancy are install on your system:

sudo apt install clang

sudo apt install make

sudo apt install libglew-dev

sudo apt install libglfw3

sudo apt install libglfw3-dev

sudo apt install clang

And finally run the make command (or run the task.json on visual studio code).

OpenGL SE:

See the OpenGL-ES-Ubuntu or OpenGL-ES-RadxaOS branch.
