
I implemented my qt app and gl code completely independant of the given source code.
I did this because I don't like working in Windows, or with IDEs (like QtCreator).

I used the code found here: https://www.thasler.org/blog/?p=36 as the base of my application.

The code builds and runs in Linux with waf, which uses python.
It should be able to build in Windows with a valid python install, but I have not tried.
To build, run "python waf configure build" from the "ex1" directory.

The executable is made in "build/demo".

Note that this application requires OpenGl 3.2 to work.
On Optimus laptops running Linux this means using bumblebee.
With bumblebee installed run "optirun build/demo" to run the program.



