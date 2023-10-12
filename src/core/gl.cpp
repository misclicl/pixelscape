#include "gl.h"
#include "display.h"


static CameraType projection_type;

/*
An example of vertex shader:

..define fs_in

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

From the example, it's evident that transformations are applied per-vertex.
Before invoking the fragment shader, the graphics pipeline processes each vertex.

Main steps in the rendering pipeline:
- Vertex shader: processes and transforms vertex attributes.
- Clipping: discards or truncates geometry outside the viewing volume. This only
  applies to gl_Position linked triangles
- Perspective division and viewport transformation: converts clip space to normalized
  device coordinates (NDC) and then to screen space.
- Rasterization: converts primitives to fragments.
- Fragment shader: computes the final color of each fragment.
*/

void render_mesh(
    TinyMesh *mesh,
    size_t shape_idx,
    ColorBuffer *color_buffer,
    DepthBuffer * depth_buffer,
    Matrix4 *mat_world,
    Vec3f camera_direction,
    VertextShader vertex_shader,
    FragmentShader fragment_shader,
    RenderFlagSet render_flags
) {
    size_t face_buffer_idx = 0;

    int target_half_width = color_buffer->width / 2;
    int target_half_height = color_buffer->height / 2;


    TinyTriangle projected_triangle = {};
    TinyTriangle projected_triangle_light = {};

    VertexShaderAttributes vertex_attrs;

    for (int i = 0; i < mesh->shapes[shape_idx].face_count; i++) {
        TinyFace *face = &(mesh->shapes[shape_idx].faces[i]);

        Vec4f positions[3] = {
            mesh->vertices[face->indices[0]].position,
            mesh->vertices[face->indices[1]].position,
            mesh->vertices[face->indices[2]].position
        };

        if (render_flags[CULL_BACKFACE]) {

            auto triangle_normal = get_triangle_normal(positions);
            float dot_normal_cam = Vec3f::dot(camera_direction, triangle_normal);


            if (dot_normal_cam < 0.f) {
                continue;
            }
        }

        for (int j = 0; j < 3; j++) {
            VertexShaderAttributes va = {
                .position = positions[j],
                .normal = mesh->vertices[face->indices[j]].normal,
                .tex_coords = mesh->vertices[face->indices[j]].texcoords
            };


            auto transformed_vertex = vertex_shader(&va);

            Vec4f position = transformed_vertex.frag_pos;

            if (position.w != 0.f) {
                position.x /= position.w;
                position.y /= position.w;
                position.z /= position.w;
            }

            position = {
                (position.x * target_half_width) + target_half_width,
                (position.y * target_half_height) + target_half_height,
                position.z, // z must be in NDC
                position.w,
            };

            projected_triangle.vertices[j].position = position;
            projected_triangle.vertices[j].normal = transformed_vertex.normal;
            projected_triangle.vertices[j].texcoords = transformed_vertex.tex_coords;

            projected_triangle_light.vertices[j].position = transformed_vertex.frag_pos_light_space;
        }

        // TODO: this must go to vertex shader
        // for (int v_idx = 0; v_idx < 3; v_idx++) {
        //     auto vertex_projected_light = transform_model_view(
        //         mesh->vertices[face->indices[v_idx]].position,
        //         mat_world,
        //         &camera_orthographic.view_matrix);
        //     vertex_projected_light = mat4_multiply_projection_vec4(
        //         camera_orthographic.projection_matrix,
        //         vertex_projected_light);
        //
        //     projected_triangle_light.vertices[v_idx].position = vertex_projected_light;
        // }

        depth_test(
            projection_type,
            color_buffer,
            depth_buffer,
            &projected_triangle,
            &projected_triangle_light,
            fragment_shader
        );
    }
}

void psgl_set_projection_type(CameraType type) {
    projection_type = type;
};

CameraType *psgl_get_projection_type() {
    return &projection_type;
}

