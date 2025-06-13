// GLBParser.cpp
#define TINYGLTF_IMPLEMENTATION    //ȷ��Ψһʵ��
#define TINYGLTF_USE_CPP14
#include "Core/EndianUtils.h"
#include "GLBParser.h"
#include <iostream>
#include <iostream>
#include <stdexcept>

namespace Mirror
{
    namespace GLTF
    {

Mesh GLBParser::GLBData::ToMesh() const {
    return Mesh(
        std::vector<Vertex>(vertices), // ƥ���ƶ����캯��
        std::vector<unsigned int>(indices)
    );
}
GLBParser::GLBData GLBParser::Parse(const std::vector<uint8_t>& glbData) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    // ����������
    loader.SetPreserveImageChannels(false);
    loader.SetImageLoader(nullptr, nullptr);

    
    // GLBͷУ��
    if (glbData.size() < 12) {
        throw std::runtime_error("GLB���ݹ���");
    }
    const uint32_t* glbHeader = reinterpret_cast<const uint32_t*>(glbData.data());
    // ��Ҫ�Ƚ�ָ��ת��Ϊ�ֽ���ָ��
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(glbHeader);
    const uint32_t version = Mirror::Core::EndianUtils::ReadLittleEndian<uint32_t>(bytePtr + sizeof(uint32_t) * 1); // �ȼ��� glbHeader[1]
    if (memcmp(glbHeader, "glTF", 4) != 0 || (version != 1 && version != 2)) {
        throw std::runtime_error("��֧�ֵ�GLB�汾: " + std::to_string(version));
    }
    // ģ�ͽ���
    if (!loader.LoadBinaryFromMemory(&model, &err, &warn, 
                                   glbData.data(), glbData.size())) {
        throw std::runtime_error("GLTF����ʧ��: " + err + 
                                (warn.empty() ? "" : "\n����: " + warn));
    }
    // ������
    std::cout << "[INFO] ����ģ�ͳɹ�:\n"
              << "  Mesh����: " << model.meshes.size() << "\n"
              << "  ��������: " << model.materials.size() << "\n";
    GLBData result;
    ProcessModel(model, result);
    return result;
}
// ʵ��ϸ��
void GLBParser::ProcessModel(const tinygltf::Model& model, GLBData& result) {
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES) continue;
            ProcessPrimitive(model, primitive, result);
        }
    }
    if (!model.nodes.empty() && !model.nodes[0].matrix.empty()) {
        memcpy(glm::value_ptr(result.transform), 
             model.nodes[0].matrix.data(),
             sizeof(float) * 16);
    }
}
void GLBParser::ProcessPrimitive(const tinygltf::Model& model,
                               const tinygltf::Primitive& primitive,
                               GLBData& result) {
    // �������Դ���
    const auto& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
    const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
    
    // �����ѡUV
    auto uvAttr = primitive.attributes.find("TEXCOORD_0");
    const float* uvs = (uvAttr != primitive.attributes.end()) ?
        GetBufferData<float>(model, model.accessors[uvAttr->second]) : nullptr;
    // �����������
    const size_t baseIndex = result.vertices.size();
    result.vertices.resize(baseIndex + posAccessor.count);
    const float* positions = GetBufferData<float>(model, posAccessor);
    const float* normals = GetBufferData<float>(model, normalAccessor);
    for (size_t i = 0; i < posAccessor.count; ++i) {
        Vertex& v = result.vertices[baseIndex + i];
        v.Position = { positions[i*3], positions[i*3+1], positions[i*3+2] };
        v.Normal = { normals[i*3], normals[i*3+1], normals[i*3+2] };
        v.TexCoords = uvs ? glm::vec2(uvs[i*2], uvs[i*2+1]) : glm::vec2(0);
    }
    // ��������
    const auto& indexAccessor = model.accessors[primitive.indices];
    switch (indexAccessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            AppendIndices<uint32_t>(model, indexAccessor, result.indices, baseIndex);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            AppendIndices<uint16_t>(model, indexAccessor, result.indices, baseIndex);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            AppendIndices<uint8_t>(model, indexAccessor, result.indices, baseIndex);
            break;
        default:
            throw std::runtime_error("��֧�ֵ���������");
    }
}
// ģ����ʽʵ����
template const float* 
GLBParser::GetBufferData<float>(const tinygltf::Model&, const tinygltf::Accessor&);
template void 
GLBParser::AppendIndices<uint32_t>(const tinygltf::Model&,
                                 const tinygltf::Accessor&,
                                 std::vector<unsigned int>&,
                                 size_t);
template void 
GLBParser::AppendIndices<uint16_t>(const tinygltf::Model&,
                                 const tinygltf::Accessor&,
                                 std::vector<unsigned int>&,
                                 size_t);
template void 
GLBParser::AppendIndices<uint8_t>(const tinygltf::Model&,
                                const tinygltf::Accessor&,
                                std::vector<unsigned int>&,size_t);

   }
}
