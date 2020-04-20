#include "pch.hpp"
#include "Settings.hpp"
#include "ByteBuffer.hpp"
#include "IO.hpp"
#include "yaml-cpp/yaml.h"

namespace turd
{
    Settings::Settings()
    {
        YAML::Node config = YAML::Load(IO::File("config.yml")->ToString());

        auto window = config["window"];
        mWindowSettings.Fullscreen = window["fullscreen"].as<bool>();
        mWindowSettings.Maximized = window["maximized"].as<bool>();
        mWindowSettings.Width = window["width"].as<uint32_t>();
        mWindowSettings.Height = window["height"].as<uint32_t>();
        mWindowSettings.Title = window["title"].as<std::string>();

        auto renderer = config["renderer"];
        mRendererSettings.Debug = renderer["debug"].as<bool>();
        mRendererSettings.VSync = renderer["vsync"].as<bool>();
        mRendererSettings.Samples = renderer["samples"].as<uint32_t>();
    }

    Settings::~Settings() {}

    Settings::Renderer &Settings::RendererSettings() { return mRendererSettings; }

    Settings::Window &Settings::WindowSettings() { return mWindowSettings; }

    void Settings::Save()
    {
        YAML::Node root;
        YAML::Node window;

        window["title"] = mWindowSettings.Title;
        window["width"] = mWindowSettings.Width;
        window["height"] = mWindowSettings.Height;
        window["maximized"] = mWindowSettings.Maximized;
        window["fullscreen"] = mWindowSettings.Fullscreen;

        YAML::Node renderer;
        renderer["debug"] = mRendererSettings.Debug;
        renderer["vsync"] = mRendererSettings.VSync;
        renderer["samples"] = mRendererSettings.Samples;

        root["window"] = window;
        root["renderer"] = renderer;

        std::cout << root;
    }

    Settings &GetSettings()
    {
        static Settings gSettings;
        return gSettings;
    }
} // namespace turd
