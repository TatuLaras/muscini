# muscini - a different kind of music visualizer

![2025-12-02 13-52-59(1)](https://github.com/user-attachments/assets/75426de6-4576-4360-85ae-0bdc40c5d857)

As with many of my projects the idea for this one came from a need of mine.
I've been a fan of 90s to early 00s demoscene demos and I wanted a to do something like that myself.
I wanted to have a sort of tool for creative visual programming that allows you to put on some psytrance and get "in the zone" programming a cool visualization through graphics programming that could incorporate information about currently playing desktop audio.

Later I also added support for JACK audio kit, meaning that the visualizer could also work in a low latency environment.
I realized I could program a visualization and make music inspired by it while having it on screen, or vice versa.
When you're done you would basically have a ready-made music video if you want to upload it to YouTube for instance.
At the time of writing I haven't done a visual inspired song or a song inspired visual but this interplay of visuals and sound would be *really* interesting to explore in the future.

Preamble over.

## Dependencies
- raylib

## Features

### Instant iteration and feedback
To really get in a "creative flow", removing all obstacles between the contents of your brain and the actual implementation is crucial.
The ideal situation is to type something, hit a button and see results instantly, which is what I did for this project.
Each visualization is loaded as a dynamic library and instantly reloaded on new changes using my hot-reloading library [firewatch](https://github.com/TatuLaras/firewatch), allowing you to write C code like a scripting language and instantly see the results in the visualization.
Firewatch can also be used inside of the visualizations themselves in order to hot-reload shaders or other resources.

Currently compilation is not handled by the hot reloading system (you see changes after running `make scenes` in the project root), as I prefer to just have a keymap in my editor to run that command. If anyone has a need for such feature let me know.

### JACK audio kit
This allows you to have muscini as a part of your realtime pro-audio environment, with low-latency.
This has interesting applications ranging from having a visualization as inspiration to live visuals for gigs etc.

It also accepts MIDI input.
Currently there is one input for the "beat" of the song, meaning that for every note-on message the visualization will react to a "beat".
Usually a beat is detected from the audio signal but by mapping this input to for instance your kick drum track you can have a more accurate beat.
There are plans to explore accepting more types of MIDI input into muscini.

## TODO features
- Slideshow mode
- Output straight to a video file
- Additional audio analysis techniques and metrics
- Additional MIDI integration, maybe control change messages? Endless possibilities :)


## Installation and usage

Install muscini by running:
```shell
# make install
```

Muscini will be installed into `/usr/bin/muscini`.

The visualizations themselves are written inside `/scene_src`.
They can be built using:
```shell
$ make scenes
```
after which the visualization files will be in `build/scenes`.

### Basic usage
Muscini at minimum needs a visualization file path to be provided:

```shell
$ muscini build/scenes/my_scene.so
```

This will run muscini in normal audio mode and will prompt for a device to use for audio capture.
Info on additional options can be obtained through the `--help` or `-h` flag.

### Writing visualizations
Have a look at `scene_src/basic.c`, there I have made a minimal example visualization with explanatory comments.

