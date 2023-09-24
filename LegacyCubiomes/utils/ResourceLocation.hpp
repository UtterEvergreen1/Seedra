#pragma once
#include <string>
/// Function to calculate the hash code for a string (replicating Java's hashCode)
static int javaHashCode(const std::string& str) {
    int hash = 0;
    int prime = 31;
    int index = 0;
    int n = str.length() - 1;
    while (n > 0) {
        hash += (static_cast<int>(str[index]) * prime) ^ n;
        index++;
        n--;
    }
    hash += static_cast<int>(str[index]);

    return hash;
}

struct ResourceLocation {
public:
    std::string resourceDomain;
    std::string resourcePath;
    ResourceLocation() {}
    ResourceLocation(const ResourceLocation& other) {
        resourceDomain = other.resourceDomain;
        resourcePath = other.resourcePath;
    }
    ResourceLocation(const std::string& resourcePathIn)
            : resourceDomain("minecraft"), resourcePath(resourcePathIn) {}
    ResourceLocation(const std::string& resourceDomainIn, const std::string& resourcePathIn)
            : resourceDomain(resourceDomainIn), resourcePath(resourcePathIn) {}
    bool operator==(const ResourceLocation &resourceLocation) const {
        return resourceDomain == resourceLocation.resourceDomain && resourcePath == resourceLocation.resourcePath;
    }

    std::size_t operator()(const ResourceLocation &resourceLocation) const {
        int hashValue = 31 * javaHashCode(resourceLocation.resourceDomain) + javaHashCode(resourceLocation.resourcePath);
        return hashValue;
    }
};

