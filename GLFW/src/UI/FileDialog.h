#pragma once
#include <string>

struct GLFWwindow;

namespace FileDialog {
    // Shows the native "Open File" dialog filtered to model formats (.obj/.gltf/.glb/.fbx).
    // Returns the selected path (UTF-8, forward slashes) or an empty string if the user
    // cancelled or the dialog failed. `owner` (optional) ties the dialog to the app window
    // so it doesn't float behind it. Windows-only, matching this project's platform.
    std::string openModelFile(GLFWwindow* owner = nullptr);
}
