:warning: | This is experimental tech in very early R&D.
| -- | -- |

# Low Level Renderering
Provides concrete [rive-cpp renderers](https://github.com/rive-app/rive-cpp/blob/master/include/renderer.hpp) for displaying Rive content on any platform.

# Renderer Support
- [] OpenGL
- [] Metal
- [] Vulkan
- [] D3D11
- [] D3D12

## OpenGL (ES 2.0 compliant)
Primarily for older mobile devices where OpenGL ES 2.0 is supported and more modern renderers are not. This is not strictly limited to ES 2.0 devices, however. The API surface used is only the common set between OpenGL and OpenGL ES 2.0 in order to guarantee at minimum ES 2.0 support.

## Metal
For OSX and iOS. Planned to be implemented via Objective-C for easy interop with C++.

## Vulkan
Intended for primary use on modern Android devices and Linux.

## D3D11 and D3D12
For Windows and Windows based systems (Xbox).