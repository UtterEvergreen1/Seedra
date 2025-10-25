#include "MemPool.hpp"
#include "terrain/ChunkPrimer.hpp"

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

template class MemPool<ChunkPrimer>;