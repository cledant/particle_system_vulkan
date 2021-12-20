# particle_system
![screenshot](./screenshots/particle_system_vulkan.png?raw=true)

particle_system is a particle movement simulator made with Vulkan, C++20 and Glfw.

## Features

particle_system generates a `cube` / `sphere` or `disk` filled with particles.  
When simulation is running, they will be attracted to a gravity center.  
Particles `base color` / `numbers` / `max speed` can be set with GUI.

## Compiling

Make sure to get submodules by running `git submodule init && git submodule update`.  
You also need `Vulkan drivers and headers` and `glslc` installed.  
You may compile `particle_system` binary by running `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j8`.

## Usage

Run `./particle_system`.  
There is no CLI options.  
Press `F1` to display controls.
