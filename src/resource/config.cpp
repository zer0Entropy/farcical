//
// Created by dgmuller on 6/4/25.
//
#include <fstream>
#include "../../include/resource/config.hpp"

std::expected<farcical::Config, farcical::Error> farcical::LoadConfig(std::string_view path) {
    std::ifstream configFile{std::string{path}, std::ios_base::in};
    if(!configFile.good()) {
        std::string failMsg{"Failed to load config file from " + std::string{path} + "."};
        return std::unexpected{Error{Error::Signal::InvalidPath, failMsg}};
    }
    farcical::Config config;
    config.data = nlohmann::json::parse(configFile);
    if(configFile.is_open()) {
        configFile.close();
    }
    return config;
}

std::optional<farcical::Error> farcical::SaveConfig(std::string_view path) {
    std::ofstream configFile{std::string{path}, std::ios_base::out};
    if(!configFile.good()) {
        std::string failMsg{"Failed to save config file to " + std::string{path} + "."};
        return Error{Error::Signal::WriteFailure, failMsg};
    }
    if(configFile.is_open()) {
        configFile.close();
    }
    return std::nullopt;
}
