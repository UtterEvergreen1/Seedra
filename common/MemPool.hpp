#pragma once
#include <functional>
#include <mutex>
#include <vector>

template <class T>
class MemPool {
public:
    explicit MemPool(size_t footprintBytes) : m_footprintBytes(footprintBytes) {}
    // Global bounded pool for reusing ChunkPrimer allocations across worlds
    // to reduce fragmentation and cap memory usage (16 MB default)
    void setPoolLimitBytes(size_t bytes);
    size_t getPoolLimitBytes() const { return m_poolLimitBytes; }
    size_t getPoolSizeBytes() const { return m_poolBytes; }
    void clearPool();

    // Acquire a chunk from the pool (may return nullptr if pool empty)
    T* acquireFromPool();
    // Return a chunk to the pool (may delete if pool is at capacity)
    void releaseToPool(T* value);

private:
    std::mutex m_poolMutex;
    std::vector<T*> m_pool;
    size_t m_poolBytes = 0;                 // current total cached bytes
    size_t m_poolLimitBytes = (1ull << 24); // cap for total cached bytes (16 MB default)
    size_t m_footprintBytes;                // footprint per value
};
