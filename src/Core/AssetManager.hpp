#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <memory>
#include <string>

#include "Logging.hpp"

namespace Neptune::Assets {
namespace fs = std::filesystem;
class AssetManager {
public:
  AssetManager() : m_AssetDirectory("../assets/") { Init(); }
  explicit AssetManager(fs::path asset_dir) : m_AssetDirectory(asset_dir) {
    Init();
  }
  ~AssetManager() = default;

  AssetManager(const AssetManager &) = delete;
  AssetManager &operator=(const AssetManager &) = delete;
  AssetManager(AssetManager &&) = delete;
  AssetManager &operator=(AssetManager &&) = delete;

  void Init() {
    if (!fs::exists(m_AssetDirectory) || !fs::is_directory(m_AssetDirectory))
      NEPTUNE_FATAL("Asset directortory not found!");

    for (const auto &entry : fs::directory_iterator(m_AssetDirectory)) {
      if (entry.is_directory())
        continue;

      auto ext = entry.path().extension().string();
      if (ext == ".png" || ext == ".jpg") {
        auto texture = std::make_shared<sf::Texture>();
        if (texture->loadFromFile(entry.path())) {
          texture->setSmooth(true);
          std::string name = entry.path().stem().string();
          m_Textures[name] = std::move(texture);
          NEPTUNE_INFO("Loaded asset: {}", name);
        }
      }
    }
  }

  std::shared_ptr<sf::Texture> GetTexture(const std::string &name) {
    auto it = m_Textures.find(name);
    if (it == m_Textures.end())
      NEPTUNE_FATAL("AssetManager::GetTexture() Texture not found!");

    return it->second;
  }

private:
  const fs::path m_AssetDirectory;
  std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_Textures;
};
} // namespace Neptune::Assets
