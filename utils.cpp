#include "utils.hpp"

std::filesystem::path getDataPath()
{
    const char* appdir = std::getenv("APPDIR");

    if (appdir)
    {
        return std::filesystem::path(appdir)
             / "usr/share/files";
    }

    // Fallback for running outside AppImage
    return "./";
}