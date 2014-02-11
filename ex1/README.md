
I implemented my qt app and gl code completely independant of the given source code.
I did this because I don't like working in Windows, or with IDEs (like QtCreator).

I used the code found here: https://www.thasler.org/blog/?p=36 as the base of my application.

The code builds and runs in Linux with waf, which uses python.
It should be able to build in Windows with a valid python install, but I have not tried.
To build, run "python waf configure build" from the "ex1" directory.

The executable is made in "build/wireframe".

Note that this application requires OpenGl 3.2 to work.
On Optimus laptops running Linux this means using bumblebee.
With bumblebee installed run "optirun build/wireframe" to run the program.


Part 1

Im still working on setting up texturing myself...

Part 2

See suzanne.png for a screen shot of my working shaders for the wireframe.

Part 3

I did not know what the normal matrix was, so I looked it up and found this, which gave away the answer:
http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
With normal matrix N and model view matrix MV, N = (MV^-1)^T.

in is an input
out is an output
uniform is the same for the entire model

The geometry shader has access to all 3 vertices which are needed for distance calculations.

A vertex attribute is an attribute associated with each vertex, like texture coordinates or vertex normals.

In immediate mode the application is completely responsible for rendering.  In retained mode the graphics library is responsible for redrawing the scene when it is changed by the application. 

A stateful API is operated by telling the API to make state transitions.
A stateless API does what asked when asked without being affected by previous calls into the API.



