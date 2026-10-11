#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// Renders one Mesh many times, each with its own model matrix and base color.
// Reuses the source mesh's VBO/EBO. Allocates one extra VBO for the
// per-instance data (16 floats matrix + 3 floats color = 19 floats/instance).
struct InstancedMesh {
    GLuint vao = 0;
    GLuint instanceVBO = 0;
    GLuint srcVAO = 0;
    GLsizei indexCount = 0;
    GLsizei instanceCount = 0;

    InstancedMesh() = default;

    // Attaches to an existing mesh's VAO and allocates the instance buffer.
    void init(const struct Mesh& src, size_t maxInstances) {
        srcVAO = src.vao;
        indexCount = src.indexCount;

        // Reuse the mesh's VAO so we don't duplicate the vertex attribs.
        vao = srcVAO;
        glBindVertexArray(vao);

        // Instance buffer: 16 floats (mat4) + 3 floats (color) per instance.
        const GLsizei stride = sizeof(float) * 19;
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(float) * 19 * maxInstances,
                     nullptr, GL_DYNAMIC_DRAW);

        // Locations 3,4,5,6 = matrix columns; 7 = color.
        for (int i = 0; i < 4; ++i) {
            GLuint loc = 3 + i;
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride,
                                  (void*)(sizeof(float) * (i * 4)));
            glVertexAttribDivisor(loc, 1);
        }
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, stride,
                              (void*)(sizeof(float) * 16));
        glVertexAttribDivisor(7, 1);

        glBindVertexArray(0);
    }

    // Upload the instance array and remember the count.
    void upload(const std::vector<glm::mat4>& models,
                const std::vector<glm::vec3>& colors) {
        instanceCount = (GLsizei)std::min(models.size(), colors.size());
        std::vector<float> flat;
        flat.reserve(instanceCount * 19);
        for (GLsizei i = 0; i < instanceCount; ++i) {
            const float* m = glm::value_ptr(models[i]);
            flat.insert(flat.end(), m, m + 16);
            const float* c = glm::value_ptr(colors[i]);
            flat.insert(flat.end(), c, c + 3);
        }
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        flat.size() * sizeof(float), flat.data());
    }

    void draw() const {
        if (instanceCount == 0) return;
        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, indexCount,
                                GL_UNSIGNED_INT, nullptr, instanceCount);
        glBindVertexArray(0);
    }
};
