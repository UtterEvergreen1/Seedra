#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <functional>

class ResourceLocation {

protected:
    std::string resourceDomain;
    std::string resourcePath;

    ResourceLocation(int unused, std::vector<std::string> resourceName) {
        this->resourceDomain = "minecraft";
        /*org.apache.commons.lang3.StringUtils.isEmpty(resourceName[0])
            ? "minecraft" : resourceName[0].toLowerCase(Locale.ROOT);*/
        this->resourcePath = resourceName[1];
        std::transform(this->resourcePath.begin(), this->resourcePath.end(),
                       this->resourcePath.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        // Validate.notNull(this->resourcePath);
    }

public:
    explicit ResourceLocation(const std::string& resourceName)
        : ResourceLocation(0, splitObjectName(resourceName)) {}

    ResourceLocation(const std::string& resourceDomainIn, const std::string& resourcePathIn)
        : ResourceLocation(0, {resourceDomainIn, resourcePathIn}) {}

protected:
    static std::vector<std::string> splitObjectName(const std::string& toSplit) {
        std::vector<std::string> aString = {"minecraft", toSplit};
        size_t i = toSplit.find(':');

        if (i != std::string::npos) {
            aString[1] = toSplit.substr(i + 1);

            if (i > 1) {
                aString[0] = toSplit.substr(0, i);
            }
        }

        return aString;
    }

public:
    [[nodiscard]] std::string getResourcePath() const { return this->resourcePath; }

    [[nodiscard]] std::string getResourceDomain() const { return this->resourceDomain; }

    [[nodiscard]] std::string toString() const { return this->resourceDomain + ':' + this->resourcePath; }

    // operator== replaces equals()
    bool operator==(const ResourceLocation& other) const {
        return this->resourceDomain == other.resourceDomain &&
               this->resourcePath == other.resourcePath;
    }

    // operator!= for completeness
    bool operator!=(const ResourceLocation& other) const {
        return !(*this == other);
    }

    // operator< replaces compareTo() (for std::map, std::set, sorting, etc.)
    bool operator<(const ResourceLocation& other) const {
        if (this->resourceDomain != other.resourceDomain) {
            return this->resourceDomain < other.resourceDomain;
        }
        return this->resourcePath < other.resourcePath;
    }

    // Additional comparison operators for completeness
    bool operator<=(const ResourceLocation& other) const {
        return !(other < *this);
    }

    bool operator>(const ResourceLocation& other) const {
        return other < *this;
    }

    bool operator>=(const ResourceLocation& other) const {
        return !(*this < other);
    }
};

// Hash function for std::unordered_map, std::unordered_set
// This replaces hashCode()
namespace std {
    template<>
    struct hash<ResourceLocation> {
        size_t operator()(const ResourceLocation& rl) const {
            // Combine hashes similar to Java's hashCode implementation
            size_t h1 = std::hash<std::string>{}(rl.getResourceDomain());
            size_t h2 = std::hash<std::string>{}(rl.getResourcePath());
            return h1 * 31 + h2;
        }
    };
}
