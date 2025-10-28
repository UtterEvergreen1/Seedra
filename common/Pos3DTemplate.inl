#pragma once

template<class T>
bool Pos3DTemplate<T>::operator==(const Pos3DTemplate &other) const { return x == other.x && z == other.z; }

template<class T>
void Pos3DTemplate<T>::operator+=(const Pos2DTemplate<T>& other) {
    x += other.x;
    z += other.z;
}

template<class T>
void Pos3DTemplate<T>::operator-=(const Pos2DTemplate<T>& other) {
    x -= other.x;
    z -= other.z;
}

template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator+(const Pos3DTemplate &other) const {
    return {x + other.x, y + other.y, z + other.z};
}

template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator+(const T other) const {
    return {x + other, y + other, z + other};
}
template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator+(const Pos2DTemplate<T>& other) const {
    return {x + other.x, y, z + other.z};
}

template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator-(const Pos3DTemplate &other) const {
    return {x - other.x, y - other.y, z - other.z};
}


template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator-(const T other) const {
    return {x - other, y - other, z - other};
}

template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::operator-(const Pos2DTemplate<T>& other) const {
    return {x - other.x, y, z - other.z};
}

template<class T>
bool Pos3DTemplate<T>::operator>(int value) const { return x > value && y > value && z > value; }


template<class T>
bool Pos3DTemplate<T>::operator<(int value) const { return x < value && y < value && z < value; }


template<class T>
bool Pos3DTemplate<T>::operator>=(int value) const { return x >= value && y >= value && z >= value; }


template<class T>
bool Pos3DTemplate<T>::operator<=(int value) const { return x <= value && y <= value && z <= value; }

template<class T>
Pos3DTemplate<T> Pos3DTemplate<T>::offset(const EnumFacing facing, const int n) const {
    switch (facing) {
        case EnumFacing::NORTH:
            return {x, y, z - n};
        case EnumFacing::SOUTH:
            return {x, y, z + n};
        case EnumFacing::WEST:
            return {x - n, y, z};
        case EnumFacing::EAST:
            return {x + n, y, z};
        case EnumFacing::UP:
            return {x, y + n, z};
        case EnumFacing::DOWN:
            return {x, y - n, z};
        default:
            return *this;
    }
}


template<class T>
ND double Pos3DTemplate<T>::distanceSqXZ() const {
    using ValueType = std::conditional_t<std::is_same_v<T, double>, T, double>;

    auto d0 = static_cast<ValueType>(x);
    auto d1 = static_cast<ValueType>(z);

    return d0 * d0 + d1 * d1;
}


template<class T>
ND double Pos3DTemplate<T>::distanceSq() const {
    using ValueType = std::conditional_t<std::is_same_v<T, double>, T, double>;

    auto d0 = static_cast<ValueType>(x);
    auto d1 = static_cast<ValueType>(y);
    auto d2 = static_cast<ValueType>(z);

    return d0 * d0 + d1 * d1 + d2 * d2;
}



template<class T>
MU double Pos3DTemplate<T>::distanceSq(c_double toX, c_double toY, c_double toZ) const {
    /*
    c_double d0 = static_cast<double>(x) - toX;
    c_double d1 = static_cast<double>(y) - toY;
    c_double d2 = static_cast<double>(z) - toZ;
    return d0 * d0 + d1 * d1 + d2 * d2;
    */
    using ValueType = std::conditional_t<std::is_same_v<T, double>, T, double>;

    constexpr auto cast = [](auto value) -> ValueType {
        return static_cast<ValueType>(value);
    };

    ValueType d0 = cast(x) - toX;
    ValueType d1 = cast(y) - toY;
    ValueType d2 = cast(z) - toZ;
    return d0 * d0 + d1 * d1 + d2 * d2;
}

template<class T>
Pos2DTemplate<T> Pos3DTemplate<T>::asPos2D() const { return {x, z}; }

template<class T>
template<typename, typename>
Pos3DTemplate<T> Pos3DTemplate<T>::convertToChunkCoords() const {
    return {x & 15, y & 255, z & 15};
}


template<class T>
std::vector<Pos3DTemplate<T>> Pos3DTemplate<T>::getAllInBox(
    const Pos3DTemplate &from, const Pos3DTemplate &to) {
    c_int minX = std::min(from.getX(), to.getX());
    c_int minY = std::min(from.getY(), to.getY());
    c_int minZ = std::min(from.getZ(), to.getZ());
    c_int maxX = std::max(from.getX(), to.getX());
    c_int maxY = std::max(from.getY(), to.getY());
    c_int maxZ = std::max(from.getZ(), to.getZ());

    std::vector<Pos3DTemplate> positions((maxX - minX + 1) * (maxY - minY + 1) * (maxZ - minZ + 1));
    int posIndex = 0;
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                positions[posIndex++] = {static_cast<T>(x), static_cast<T>(y), static_cast<T>(z)};
            }
        }
    }
    return positions;
}



template<class T>
ND double Pos3DTemplate<T>::dot(const Pos3DTemplate &other) const {
    using ValueType = std::conditional_t<std::is_same_v<T, double>, T, double>;
    const auto ax = static_cast<ValueType>(x);
    const auto ay = static_cast<ValueType>(y);
    const auto az = static_cast<ValueType>(z);
    const auto bx = static_cast<ValueType>(other.x);
    const auto by = static_cast<ValueType>(other.y);
    const auto bz = static_cast<ValueType>(other.z);
    return static_cast<double>(ax * bx + ay * by + az * bz);
}

template<class T>
MU ND Pos3DTemplate<double>
Pos3DTemplate<T>::closestPointOnSegment(const Pos3DTemplate &segStart,
                                        const Pos3DTemplate &segEnd) const {
    // Cast everything to double for robust geometry
    const auto sx = static_cast<double>(segStart.x);
    const auto sy = static_cast<double>(segStart.y);
    const auto sz = static_cast<double>(segStart.z);

    const auto ex = static_cast<double>(segEnd.x);
    const auto ey = static_cast<double>(segEnd.y);
    const auto ez = static_cast<double>(segEnd.z);

    const auto px = static_cast<double>(x);
    const auto py = static_cast<double>(y);
    const auto pz = static_cast<double>(z);

    // v1 = P - S, v2 = E - S
    const double v1x = px - sx, v1y = py - sy, v1z = pz - sz;
    const double v2x = ex - sx, v2y = ey - sy, v2z = ez - sz;

    const double dot = v1x * v2x + v1y * v2y + v1z * v2z;
    const double len2 = v2x * v2x + v2y * v2y + v2z * v2z;

    // Degenerate segment (S==E) or projects before start
    if (len2 <= 0.0 || dot <= 0.0) {
        return {sx, sy, sz};
    }

    // Projects beyond end
    if (dot >= len2) {
        return {ex, ey, ez};
    }

    // Interior projection
    const double t = dot / len2;
    return {sx + t * v2x, sy + t * v2y, sz + t * v2z};
}

template<class T>
ND double Pos3DTemplate<T>::distanceToSegment(const Pos3DTemplate &segStart,
                                              const Pos3DTemplate &segEnd) const {
    const auto cp = closestPointOnSegment(segStart, segEnd);
    return std::sqrt(distanceSq(cp.x, cp.y, cp.z));
}







template
class Pos3DTemplate<int>;

template
class Pos3DTemplate<double>;

template Pos3DTemplate<int> Pos3DTemplate<int>::convertToChunkCoords<int, void>() const;
