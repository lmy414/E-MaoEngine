// OBJLoader.h
#pragma once
#include "Render/Mesh.h"
#include <unordered_map>
#include <string>
class OBJLoader {
public:
    static Mesh LoadFromFile(const std::string& path);
    
private:
    struct VertexKey {
        int v, vt, vn;
        
        bool operator==(const VertexKey& other) const {
            return v == other.v && vt == other.vt && vn == other.vn;
        }
    };
    
    struct KeyHash {
        size_t operator()(const VertexKey& k) const {
            return ((std::hash<int>()(k.v) ^ 
                   (std::hash<int>()(k.vt) << 1)) >> 1) ^ 
                   (std::hash<int>()(k.vn) << 1);
        }
    };
};
