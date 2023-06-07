# Emscripten Browser Clipboard Demo
This is a simple demonstration of the [emscripten-browser-clipboard](https://github.com/Armchair-Software/emscripten-browser-clipboard) library.

## Live demo

Try it out in your browser: https://armchair-software.github.io/emscripten-clipboard-demo/

## Testing locally

### Requirements:
- Emscripten, environment activated
- [ImGui](https://github.com/ocornut/imgui) (snapshot version included)
- [VectorStorm](https://github.com/Armchair-Software/vectorstorm) (snapshot subset included)

### Build:

    mkdir build
    cd build
    emcmake cmake ..
    emmake make -j$(nproc)

### Run:

    emrun client.html
