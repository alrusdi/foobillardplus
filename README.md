## FooBillard++ - based on [foobillard 3.0a](https://en.wikipedia.org/wiki/FooBillard) by Florian Berger

## License

Copyright (C) 2001 Florian Berger (foobillard)

Copyright (C) 2010/2011 Holger Schaekel (foobillard++)

email: foobillardplus@go4more.de
			    
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as
published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

## What is FooBillard++?

FooBillard++ is a free OpenGL-billiard game for Linux and based on the
original foobillard 3.0a source from `Florian Berger`. 

### Why foo?

Well, actually he had this logo (F.B.-Florian Berger) and then foo sounds a
bit like pool (Somehow he wasn't quite attracted by the name `FoolBillard`)

If you are a billiard-pro and you're missing some physics,
please tell us. Cause I've implemented it like I think it should work,
which might differ from reality. Please contact us over sourceforge.net

Started at 12/2010 an advanced version is in progress with foobillard++.
It's not really a new game, but an advanced with a lot of fixes, new
options, graphics and features until the last version of foobillard (2007).
At this point it's started with a special version for the Touch-PC WeTab.

### Dependencies

You need to have `SDL` installed on your system.
Furthermore `libpng` has to be installed for loading the textures.
And `freetype2` is used for font rendering.
`SDL_net` is used for the networking mode
`SDL_mixer` for sound and music

## Prerequisites:
   - 100% OpenGL compatible graphic card drivers for your operating system.
   
   - The graphic card must have 50 to 80 Megabyte Video-RAM! If the driver can't
   allocate enough graphic card memory, the game graphics is ugly and corrupt!!
   
   - Linux, WeTab, Mac OS 10.6.x or Windows OS (32 or 64 Bit) with a minimum of 10 MB
   free system memory at runtime.
   
## Credits && Thanks 

Many thanks to the band `Zentriert ins Antlitz`, specially Marc Friedrich,
for the music in the game! Have a look on their great site at: http://www.zentriertinsantlitz.de

## The project is powered by the following amazing FOSS:

- OpenGL 1
- GNU C Compiler
- GNU `Autotools`
- The `SDL` Simple Media Library
- Eclipse C/C++ IDE
- Gimp Image Manipulation Program
- Blender 3D
- Audacity
- Free 3D graphics from www.terminal26.de
- Free 3D graphics from www.scopia.es
- Free 3d graphics from www.blendswap.com

- Sourceforge.net as hoster
- freecode.com for announcement
- google code for announcement
- youtube as hoster for the video samples


### BUILDING:


#### UNIX based platforms:

Building for sound support and no Nvidia-related specs are standard.
Please don't use `--enable-nvidia` (it's only for testing.....)

For compilation from source you need the GNU `autoconf` and `automake` packages.
After unpacking the source archive change to the `foobillardplus` directory and
type the following commands (or invoke `buildme.sh`:

```sh
aclocal --force  
autoconf -f  
autoheader -f  
automake -a -c -f  
```

#### OS X:
For Mac OS are special files inside the directory OSX from root. There is also a
project file. `Frretype` and `SDL` are included there.
 simply type in main-directory (for standard-installation):


A special very fast version is build with

./configure --enable-special  
make  
make install  

If you want to enable the special WeTab Version type:

```sh 
./configure --enable-wetab  
make  
make install  
```

That version would be only run on a WeTab Tablet-PC


### configure options

`--enable-wetab`:

This builds a version for the German tablet WeTab (and only for that!!)
Please don't use other optimization flags discussed later in this document, because this option is the only one you need!!

`--enable-touch`:

This build a special version for generic touch-devices

`--enable-mathsingle=ARG`:

Compile math single precision (default=yes). If you use "no" it is compiled for math double precision. 
**Clients with mixed single or double precision are not compatible in network games**

`--enable-fastmath`:

Compile fast math routine in (default=no). With set this configure option, special
optimized math-routines for cosine, sine, tangents are used. This has nothing to do
with `SSE` intrinsics. The fast math routines are not nearly as accurate as the
standard routines, but enough for the game.

`--enable-sse=ARG`:

Compile with intrinsics SSE commands and use. With enabled SSE, the use of
`enable-mathsingle` defaults to yes. Double precision are automatically disabled.
Use `SSE` only on Intel or AMD based CPU systems!


`--enable-network=ARG`:

Compile for IP-network game support (default=yes). With no as argument, all
network support is not compiled.

`--enable-sound=ARG`:

Enable sound (default=yes). With no as argument sound support is not compiled.

`--enable-win`:

If set to yes, the source is compiling for ms-windows (32 and 64 Bit). You have to use
as runtime environment MinGW/Msys under MS-Windows.
http://sourceforge.net/projects/mingw


### config file (.foobillardrc):

You can place a config file named ".foobillardrc" in your home directory.
Windows hold the file in the directory associated with the content of the
environment variable `USERPROFILE`.

This file can contain all possible CLI arguments (without the "-" prefix; one line for each argument).
CLI arguments are parsed last, so they override the ".foobillardrc" settings. 

### Features

- Wood paneled table with gold covers and gold diamonds
- Reflections on balls
- Shadow pixmaps
- Detail switching of balls according to distance
- Zoom in/out - hold right mouse button
- FOV +/- - hold right mouse button + `CTRL`
- Rotate - hold left mouse button
- Animated cue
- Simple billiard rules for 8 and 9-ball
- Simple AI-Player
- Strength adjustment
- Eccentric hit adjustment (button2 + Shift)
- Lens flare
- CLI options
- Config file (~/.foobillardrc)
- Lightweight red/green stereo !!!!
- Sound and music (using SDL)
- Status line for info in gameplay
- Advanced hud
- Jump shots
- Advanced snipping mode
- Tournament for all games
- Basic OpenGL improvements (anisotropic, antialias)
- Full playable in bird's eye view
- Tron like game mode
- Glass balls, if you like

Press <F1> in game for a quick help!

### red-green stereo:

One picture is drawn on red channel only, the other one on the other both channels (green, blue) so you can use either a green or blue or cyan filter for one eye (left), and a red one for the other eye (right).


### Network game

IP network support is possible. Now with IPv4. IPv6 is supported in the future.
This function is heavily BETA.


Optimization problem of the GCC suite (don't use with WeTab compiling!)
-----------------------------------------------------------------------

On some systems certain `gcc` optimisations may result in unstable code. To
To output possible optimisations on the target arch, invoke:

```sh
gcc -c -Q -O3 --help=optimizers > /tmp/O3-opts
```

Check the generated file for available optimisations on your system. Please use these wisely and don't confuse these with general `configure` arguments.

The `configure` script supports the `--enable-standard` argument.

To pass customised `CFLAGS`, use `--enable-special`. No optimisation flags are set by default.

The `--enable-optimization` switch uses some special level of optimisations and will produce hopefully a stable program. Default value is `no`.


With this switch the highest optimisation level is used. Be careful: there is no
guarantee from the author that this will produce a stable program but does quicken the build time.

### KNOWN BUGS:

- Intel integrated graphic chips
- On some Intel integrated graphic chips (GMA) the game is not playable with Linux.

**You need really 100% OpenGL compatible graphic card drivers.**
    
