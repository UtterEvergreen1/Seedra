#include "TemplateManager.hpp"
#include "ResourceLocation.hpp"
#include "Template.hpp"

Template* TemplateManager::getTemplate(const ResourceLocation& id) {
    std::string s = id.getResourcePath();

    // Check if already loaded
    auto it = this->templates.find(s);
    if (it != this->templates.end()) {
        return it->second;
    }

    // Try to load from filesystem
    this->readTemplate(id);

    // Check again after loading attempt
    it = this->templates.find(s);
    if (it != this->templates.end()) {
        return it->second;
    }

    // If still not found, create new empty template
    auto* newTemplate = new Template();
    this->templates[s] = newTemplate;
    return newTemplate;
}

Template* TemplateManager::get(const ResourceLocation& id) {
    std::string s = id.getResourcePath();

    // Check if already loaded
    auto it = this->templates.find(s);
    if (it != this->templates.end()) {
        return it->second;
    }

    // Try to load from filesystem
    if (this->readTemplate(id)) {
        return this->templates[s];
    }

    return nullptr;
}

bool TemplateManager::readTemplate(const ResourceLocation& id) {

    std::string s = id.getResourcePath();
    fs::path root = fs::current_path();

    fs::path filePath = (root / this->m_baseFolder / s).make_preferred();

    if (!fs::exists(filePath)) {
        return false;
    }

    try {
        Buffer buffer = DataReader::readFile(filePath);
        this->readTemplateFromBuffer(s, buffer);
        return true;
    } catch (...) {
        return false;
    }
}

void TemplateManager::readTemplateFromBuffer(const std::string& id, Buffer& buffer) {
    DataReader reader(buffer);

    NBTBase nbtCompound = NBTBase::read(reader);
    if (nbtCompound("")) {
        nbtCompound = std::move(nbtCompound[""]);
    }

    if (nbtCompound.hasKey("DataVersion", eNBT::PRIMITIVE)) {
        nbtCompound["DataVersion"] = makeInt(500);
    }

    auto* _template = new Template();
    _template->read(nbtCompound.get<NBTCompound>());

    // Delete old template if it exists
    auto it = this->templates.find(id);
    if (it != this->templates.end()) {
        delete it->second;
    }

    this->templates[id] = _template;
}

bool TemplateManager::writeTemplate(const ResourceLocation& id) {
    namespace fs = std::filesystem;

    std::string s = id.getResourcePath();

    // Check if template exists in memory
    auto it = this->templates.find(s);
    if (it == this->templates.end()) {
        return false;
    }

    // Ensure base folder exists
    fs::path baseDir(this->m_baseFolder);

    if (!fs::exists(baseDir)) {
        try {
            if (!fs::create_directories(baseDir)) {
                return false;
            }
        } catch (...) {
            return false;
        }
    } else if (!fs::is_directory(baseDir)) {
        return false;
    }

    // Write to file
    fs::path filePath = baseDir / (s + ".nbt");
    Template const* _template = it->second;

    try {
        std::ofstream outputStream(filePath, std::ios::binary);
        if (!outputStream.is_open()) {
            return false;
        }

        // TODO: Implement NBT writing based on your NBT library
        // NBTCompound nbtCompound = _template->writeToNBT();
        // CompressedStreamTools::writeCompressed(nbtCompound, outputStream);

        outputStream.close();
        return true;
    } catch (...) {
        return false;
    }
}

void TemplateManager::remove(const ResourceLocation& resourceLoc) {
    auto it = this->templates.find(resourceLoc.getResourcePath());
    if (it != this->templates.end()) {
        delete it->second;
        this->templates.erase(it);
    }
}