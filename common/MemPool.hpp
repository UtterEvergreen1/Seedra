#pragma once

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


template <class T>
void MemPool<T>::setPoolLimitBytes(size_t bytes) {
    std::lock_guard<std::mutex> lk(m_poolMutex);
    m_poolLimitBytes = bytes;
    // Evict until within limit
    while (m_poolBytes > m_poolLimitBytes && !m_pool.empty()) {
        T *c = m_pool.back();
        m_pool.pop_back();
        m_poolBytes -= std::min(m_poolBytes, m_footprintBytes);
        delete c;
    }
}

template <class T>
void MemPool<T>::clearPool() {
    std::lock_guard<std::mutex> lk(m_poolMutex);
    for (T *c: m_pool) delete c;
    m_pool.clear();
    m_poolBytes = 0;
}

template <class T>
T *MemPool<T>::acquireFromPool() {
    std::lock_guard<std::mutex> lk(m_poolMutex);
    if (m_pool.empty()) return nullptr;
    T *c = m_pool.back();
    m_pool.pop_back();
    m_poolBytes -= std::min(m_poolBytes, m_footprintBytes);
    return c;
}

template <class T>
void MemPool<T>::releaseToPool(T *value) {
    if (!value) return;
    // Ensure the chunk is reset before pooling to avoid stale data surprises later
    std::lock_guard<std::mutex> lk(m_poolMutex);
    if (m_poolBytes + m_footprintBytes <= m_poolLimitBytes) {
        m_pool.push_back(value);
        m_poolBytes += m_footprintBytes;
    } else {
        delete value;
    }
}
