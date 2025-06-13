// OBJLoader.cpp
#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

using namespace std;

Mesh OBJLoader::LoadFromFile(const std::string& path) {
    ifstream file(path);
    if (!file.is_open()) throw runtime_error("Failed to open OBJ file");

    vector<glm::vec3> positions;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;
    
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unordered_map<VertexKey, unsigned int, KeyHash> vertexMap;

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") { // 顶点位置
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt") { // 纹理坐标
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y; // 适配OpenGL坐标系
            texCoords.push_back(uv);
        }
        else if (type == "vn") { // 法线向量
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (type == "f") { // 面处理
            vector<VertexKey> faceVertices;
            string vertexStr;
            
            while (iss >> vertexStr) {
                VertexKey key{-1, -1, -1};
                replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                istringstream viss(vertexStr);
                
                viss >> key.v;
                if (viss.peek() != EOF) viss >> key.vt;
                if (viss.peek() != EOF) viss >> key.vn;

                // OBJ索引转换为0基
                key.v--;  
                if (key.vt >= 0) key.vt--;
                if (key.vn >= 0) key.vn--;

                // 顶点去重处理
                auto it = vertexMap.find(key);
                if (it != vertexMap.end()) {
                    faceVertices.push_back(key);
                }
                else {
                    Vertex vertex;
                    vertex.Position = positions[key.v];
                    
                    if (key.vt >= 0 && key.vt < texCoords.size()) 
                        vertex.TexCoords = texCoords[key.vt];
                    else
                        vertex.TexCoords = glm::vec2(0.0f);
                    
                    if (key.vn >= 0 && key.vn < normals.size())
                        vertex.Normal = normals[key.vn];
                    else
                        vertex.Normal = glm::vec3(0.0f);
                    
                    vertices.push_back(vertex);
                    unsigned int index = static_cast<unsigned int>(vertices.size() - 1);
                    vertexMap[key] = index;
                    faceVertices.push_back(key);
                }
            }

            // 三角面分割（支持多边形面）
            for (size_t i = 2; i < faceVertices.size(); ++i) {
                indices.push_back(vertexMap[faceVertices[0]]);
                indices.push_back(vertexMap[faceVertices[i-1]]);
                indices.push_back(vertexMap[faceVertices[i]]);
            }
        }
    }

    // 自动计算法线（如果缺失）
    bool hasNormals = !normals.empty();
    if (!hasNormals) {
        vector<vector<glm::vec3>> faceNormals(vertices.size());
        
        for (size_t i = 0; i < indices.size(); i += 3) {
            Vertex& v0 = vertices[indices[i]];
            Vertex& v1 = vertices[indices[i+1]];
            Vertex& v2 = vertices[indices[i+2]];
            
            glm::vec3 edge1 = v1.Position - v0.Position;
            glm::vec3 edge2 = v2.Position - v0.Position;
            glm::vec3 normal = normalize(cross(edge1, edge2));
            
            faceNormals[indices[i]].push_back(normal);
            faceNormals[indices[i+1]].push_back(normal);
            faceNormals[indices[i+2]].push_back(normal);
        }
        
        for (size_t i = 0; i < vertices.size(); ++i) {
            glm::vec3 sum(0.0f);
            for (auto& n : faceNormals[i]) sum += n;
            vertices[i].Normal = normalize(sum);
        }
    }

    return Mesh(move(vertices), move(indices));
}

