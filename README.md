<img src="./media/logo.png" alt="Dynamo Engine" width="500"/>

![Linux Build](https://github.com/SirBob01/Dynamo-Engine/actions/workflows/linux.yml/badge.svg)

---

`Dynamo` is a C++ game engine powered by `Vulkan`.

## Features

- Native support for an optimized [ECS](https://en.wikipedia.org/wiki/Entity_component_system) architecture in designing game objects
- Rendering engine powered by the Vulkan API
- An audio engine that supports multiple playback tracks and raw PCM data manipulation

## Dependencies

To use `Dynamo`, the compiler must support at least C++17.

`Dynamo` uses a number of submodules that can be installed by running `git submodule update --init`

## Running Demos

The engine comes with demo programs to test core features of the engine. To compile them, `cmake` must be installed. Go to the `/demos/build` directory, then run:

- `cmake .. -G BUILD_GENERATOR`
- `make`

Replace `BUILD_GENERATOR` with the [generator](https://cmake.org/cmake/help/v3.2/manual/cmake-generators.7.html) applicable to your development environment. This assumes that all the dependency requirements are satisfied.

## Running Tests

Similar to the demos, the tests need to be built. Go to the `/tests/build/` directory then run:

- `cmake .. -G BUILD_GENERATOR`
- `make`
- `ctest --verbose`

## License

Code and documentation Copyright (c) 2019-2022 Keith Leonardo

Code released under the [MIT License](https://choosealicense.com/licenses/mit/).
