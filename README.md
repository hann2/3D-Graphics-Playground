
3D Graphics Playground contains implementations of many graphics techniques.

 * Wireframe rendering implemented with shaders.
 * 3D turtle graphics for tree generation.
 * Perlin noise for texture and terrain generation.
 * Deferred rendering.
 * Toon shading (using deferred rendering).
 * Screen space ambient occlusion, or SSAO (using deferred rendering).
 * Hard shadow mapping.
 * Bump mapping.
 * Parallax mapping.
 * Particle simulation.


The code builds and runs in Linux with waf, which uses python.
It should be able to build in Windows with a valid python install, but I have not tried.
To build, run "python waf configure build".

The executable is made in "build/demo".

Note that this application requires OpenGl 3.2 to work.
On Optimus laptops running Linux this means using bumblebee.
With bumblebee installed run "optirun build/demo" to run the program.


Future additions include
 * Water shading.
 * Water surface simulation.
 * Displacement animation.
 * Boid flocking.
 * Rigged animation.
 * Catmull-clark subdivision.
