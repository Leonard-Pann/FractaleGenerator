Fractal Generator is a program dedicated to the visualization of Julia fractals in C++ using OpenGL.
The application displays beautiful Julia fractals in real time. The selection of fractals is random, although an algorithm ensures that only visually interesting ones are chosen.
The selection and generation of fractals are highly optimized, using multithreading and GPU computation via OpenGL.
The program offers dynamic animation of Julia fractals by interpolating them using Bézier curves.

![Fractal Generator](https://github.com/user-attachments/assets/3d70dc13-39cd-4115-ad18-476f0f5a8939)

Install process : \n

OpenGL 4.6 :\n

Windows :\n
Clone the main branch and open the solution with visual studio and build the projet (require Desktop developement with C++ module).

Linux :\n
Clone the linux branch and make sure all dependancy are install on your system:\n
sudo apt install clang\n
sudo apt install make\n
sudo apt install libglew-dev\n
sudo apt install libglfw3\n
sudo apt install libglfw3-dev\n
sudo apt install clang\n

OpenGL SE 3.2:\n

Clone the OpenGL 3.2 branch and make sure all dependancy are install on your system:\n
sudo apt install clang\n
sudo apt install make\n
