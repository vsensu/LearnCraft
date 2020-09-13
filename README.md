# Build

## Prerequisites
Make sure you have this library downloaded somewhere on your machine.

https://www.sfml-dev.org/download/sfml/2.5.1/

For visual studio 2019. https://share.weiyun.com/AvoFEBJ2

## Step 1: Include directories

1. Ensure that "All Configurations" is selected from the drop-down at the top of the modal.
2. In the property pages, go to `C/C++ > General`, click the `Additional Include Directories` and click on the arrow, then click `<Edit>`

This will open up another window.

In here, you'll need to add include directories:

1. Where you installed SFML, add the `include`folder

Click "OK".

Keep the Property Pages modal open.

## Step 2: Linker directories

Visual Studio needs to know where SFML libs are.

1. Ensure that "All Configurations" is selected from the drop-down at the top of the modal.
2. Go to `Linker > General`
3. In the `Additional Library Directories` row, add the `SFML/build/lib` directory:

Keep the modal open

## Step 3: Link SFML

You'll now need to link the SFML libraries.

### Debug Mode

Go to `Linker > Input` and set the configuration mode to "Debug":

Open `Additional Dependencies`, copy paste these into the text box

```
sfml-graphics-d.lib
sfml-system-d.lib
sfml-audio-d.lib
sfml-window-d.lib
sfml-network-d.lib
```

 **Make sure they are on different lines**:

### Release Mode
We now need to do the same for release mode.

1. Set the configuration mode to "Release":
2. Open `Additional Dependencies`, copy paste these into the text box
3. **Make sure they are on different lines**

```
sfml-graphics.lib
sfml-system.lib
sfml-audio.lib
sfml-window.lib
sfml-network.lib
```

You may now press OK to close the modal.

At this point, the project should compile.

However, it will not run until the last step is completed:

## Step 4: Copy SFML .dll files into the project

Open where you have SFML downloaded to, and open the `build/lib/Debug/` directory and `build/lib/Release/` directory.

You should see a bunch of .dll files.

Copy and paste all of these into the `main.cpp` file.

## Step 5

You are done!

You will now be able to build and run the project.

If you cannot, then it means a step may have been followed incorrectly.

In this case, go back through at a slower pace and make sure you did every step.

You will get there in the end :)

Enjoy!