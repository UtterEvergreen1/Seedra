#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <utility>

#include "common/data/buffer.hpp"

class ResourceLocation;
class Template;

class DataFixer {
    // Add your implementation
};

class TemplateManager {
    std::map<std::string, Template*> templates = {};
    std::string m_baseFolder;
    DataFixer field_191154_c;

public:
    TemplateManager(std::string baseFolder, DataFixer fixer)
        : m_baseFolder(std::move(baseFolder)), field_191154_c(fixer) {}

    Template* getTemplate(const ResourceLocation& id);

    Template* get(const ResourceLocation& id);

    bool readTemplate(const ResourceLocation& id);

private:
    void readTemplateFromBuffer(const std::string& id, Buffer& buffer);

public:
    MU bool writeTemplate(const ResourceLocation& id);

    void remove(const ResourceLocation& resourceLoc);
};