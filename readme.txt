So, there it is. The complete code for Teepop.

Teepop uses a library called foolib that is mostly developed by me on my spare 
time. It contains alot of diffrent stuff that I wrote just because I were 
intressted in it and there is alot of stuff there that isn't needed for the 
game.

Foolib also has it's custom project files based on xml and a python script that 
compiles them. Do build it just compress it and run the .bat/.sh file under the
wrk directory.

Windows Note! Remember that you must have cl.exe (MSVC++ compiler) in your path
when compiling. There should be a link to a MSVC Command Line under your start
menu that sets alot of settings so you can compile this. MSVC 2002 does not like
this code. You must use 2003. 

macOS Note! Building requires Python 3, Xcode Command Line Tools, pkg-config,
libvorbis, and OpenAL Soft. With Homebrew, install the audio dependencies with:

  brew install pkg-config libvorbis openal-soft

Run the build with:

  cd wrk
  ./compile.sh

The client app and server are written to wrk/output/.

Read the licence for the source. It's a zlib/libpng and I give no support or 
help what so ever with the code, but feel free to ask questions about it, maybe 
I will answer them... maybe.

.kma
