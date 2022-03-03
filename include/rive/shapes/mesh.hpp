#ifndef _RIVE_MESH_HPP_
#define _RIVE_MESH_HPP_
#include "rive/generated/shapes/mesh_base.hpp"
#include "rive/span.hpp"
#include "rive/refcnt.hpp"

namespace rive {
    class MeshVertex;

    class Mesh : public MeshBase {

    protected:
        class IndexBuffer : public std::vector<uint16_t>, public RefCnt {};
        std::vector<MeshVertex*> m_Vertices;
        rcp<IndexBuffer> m_IndexBuffer;

    public:
        StatusCode onAddedDirty(CoreContext* context) override;
        void markDrawableDirty();
        void addVertex(MeshVertex* vertex);
        void decodeTriangleIndexBytes(Span<const uint8_t> value) override;
        void copyTriangleIndexBytes(const MeshBase& object) override;
#ifdef TESTING
        std::vector<MeshVertex*>& vertices() { return m_Vertices; }
        rcp<IndexBuffer> indices() { return m_IndexBuffer; }
#endif
    };
} // namespace rive

#endif