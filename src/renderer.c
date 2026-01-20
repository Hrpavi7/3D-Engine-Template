#include "renderer.h"

#include "gl_loader.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t compile_shader(uint32_t type, const char* src) {
    GLuint s = glCreateShader_(type);
    const GLchar* strings[] = { (const GLchar*)src };
    glShaderSource_(s, 1, strings, NULL);
    glCompileShader_(s);
    GLint ok = 0;
    glGetShaderiv_(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glDeleteShader_(s);
        return 0;
    }
    return (uint32_t)s;
}

static uint32_t link_program(uint32_t vs, uint32_t fs) {
    GLuint p = glCreateProgram_();
    glAttachShader_(p, vs);
    glAttachShader_(p, fs);
    glLinkProgram_(p);
    GLint ok = 0;
    glGetProgramiv_(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        glDeleteProgram_(p);
        return 0;
    }
    return (uint32_t)p;
}

static void atlas_put_tile_rgba(uint32_t* rgba, int atlas_w, int x0, int y0, const uint32_t* tile, int tile_w, int tile_h) {
    for (int y = 0; y < tile_h; y++) {
        uint32_t* dst = rgba + (y0 + y) * atlas_w + x0;
        const uint32_t* src = tile + y * tile_w;
        memcpy(dst, src, (size_t)tile_w * sizeof(uint32_t));
    }
}

static uint32_t pack_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (uint32_t)(r) | ((uint32_t)g << 8u) | ((uint32_t)b << 16u) | ((uint32_t)a << 24u);
}

static void make_noise_tile(uint32_t* out_rgba, int w, int h, uint32_t c0, uint32_t c1, uint32_t c2) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint32_t pick = (uint32_t)((x * 1973 + y * 9277 + x * y * 26699) & 7);
            uint32_t c = (pick < 4) ? c0 : (pick < 6 ? c1 : c2);
            out_rgba[y * w + x] = c;
        }
    }
}

static uint32_t make_texture_atlas(void) {
    const int tile = 16;
    const int tiles = 4;
    const int atlas_w = tile * tiles;
    const int atlas_h = tile;

    uint32_t* rgba = (uint32_t*)malloc((size_t)atlas_w * (size_t)atlas_h * sizeof(uint32_t));
    if (!rgba) return 0;
    memset(rgba, 0, (size_t)atlas_w * (size_t)atlas_h * sizeof(uint32_t));

    uint32_t grass_top[16 * 16];
    uint32_t grass_side[16 * 16];
    uint32_t dirt[16 * 16];
    uint32_t stone[16 * 16];

    uint32_t g0 = pack_rgba(0x52, 0xA1, 0x3B, 0xFF);
    uint32_t g1 = pack_rgba(0x6B, 0xB7, 0x4D, 0xFF);
    uint32_t g2 = pack_rgba(0x3F, 0x8F, 0x2E, 0xFF);
    make_noise_tile(grass_top, tile, tile, g0, g1, g2);

    uint32_t d0 = pack_rgba(0x8A, 0x6A, 0x3D, 0xFF);
    uint32_t d1 = pack_rgba(0x7A, 0x5D, 0x34, 0xFF);
    uint32_t d2 = pack_rgba(0x9A, 0x77, 0x46, 0xFF);
    make_noise_tile(dirt, tile, tile, d0, d1, d2);

    uint32_t s0 = pack_rgba(0x86, 0x86, 0x86, 0xFF);
    uint32_t s1 = pack_rgba(0x74, 0x74, 0x74, 0xFF);
    uint32_t s2 = pack_rgba(0x96, 0x96, 0x96, 0xFF);
    make_noise_tile(stone, tile, tile, s0, s1, s2);

    for (int y = 0; y < tile; y++) {
        for (int x = 0; x < tile; x++) {
            bool top_band = (y < 6);
            uint32_t base = top_band ? grass_top[y * tile + x] : dirt[y * tile + x];
            if (top_band && ((x + y) & 7) == 0) {
                base = pack_rgba(0x7A, 0xC9, 0x63, 0xFF);
            }
            grass_side[y * tile + x] = base;
        }
    }

    atlas_put_tile_rgba(rgba, atlas_w, 0 * tile, 0, grass_top, tile, tile);
    atlas_put_tile_rgba(rgba, atlas_w, 1 * tile, 0, grass_side, tile, tile);
    atlas_put_tile_rgba(rgba, atlas_w, 2 * tile, 0, dirt, tile, tile);
    atlas_put_tile_rgba(rgba, atlas_w, 3 * tile, 0, stone, tile, tile);

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, atlas_w, atlas_h, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, rgba);
    glGenerateMipmap_(GL_TEXTURE_2D);

    free(rgba);
    return (uint32_t)tex;
}

bool renderer_init(Renderer* r) {
    memset(r, 0, sizeof(*r));

    const char* vs_src =
        "#version 330 core\n"
        "layout(location=0) in vec3 aPos;\n"
        "layout(location=1) in vec2 aUV;\n"
        "layout(location=2) in vec3 aN;\n"
        "uniform mat4 uMVP;\n"
        "out vec2 vUV;\n"
        "out vec3 vN;\n"
        "void main(){ vUV=aUV; vN=aN; gl_Position=uMVP*vec4(aPos,1.0); }\n";

    const char* fs_src =
        "#version 330 core\n"
        "in vec2 vUV;\n"
        "in vec3 vN;\n"
        "uniform sampler2D uTex;\n"
        "uniform vec3 uLightDir;\n"
        "out vec4 FragColor;\n"
        "void main(){\n"
        "  vec3 n = normalize(vN);\n"
        "  float ndl = max(dot(n, normalize(uLightDir)), 0.2);\n"
        "  vec4 c = texture(uTex, vUV);\n"
        "  FragColor = vec4(c.rgb*ndl, c.a);\n"
        "}\n";

    uint32_t vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    uint32_t fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs) return false;

    uint32_t prog = link_program(vs, fs);
    glDeleteShader_(vs);
    glDeleteShader_(fs);
    if (!prog) return false;

    r->program = prog;

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays_(1, &vao);
    glBindVertexArray_(vao);
    glGenBuffers_(1, &vbo);
    glBindBuffer_(GL_ARRAY_BUFFER, vbo);
    glBufferData_(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    GLsizei stride = (GLsizei)(8 * sizeof(float));
    glEnableVertexAttribArray_(0);
    glVertexAttribPointer_(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray_(1);
    glVertexAttribPointer_(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray_(2);
    glVertexAttribPointer_(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

    r->vao = (uint32_t)vao;
    r->vbo = (uint32_t)vbo;

    r->texture_atlas = make_texture_atlas();
    if (!r->texture_atlas) return false;

    glUseProgram_(r->program);
    int u_tex = glGetUniformLocation_(r->program, "uTex");
    r->u_mvp = glGetUniformLocation_(r->program, "uMVP");
    r->u_light_dir = glGetUniformLocation_(r->program, "uLightDir");
    glUniform1i_(u_tex, 0);
    float light[3] = { -0.6f, 1.0f, -0.2f };
    glUniform3fv_(r->u_light_dir, 1, light);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return true;
}

void renderer_shutdown(Renderer* r) {
    if (!r) return;
    if (r->texture_atlas) {
        GLuint t = (GLuint)r->texture_atlas;
        glDeleteTextures(1, &t);
    }
    if (r->vbo) {
        GLuint b = (GLuint)r->vbo;
        glDeleteBuffers_(1, &b);
    }
    if (r->vao) {
        GLuint a = (GLuint)r->vao;
        glDeleteVertexArrays_(1, &a);
    }
    if (r->program) {
        glDeleteProgram_(r->program);
    }
    memset(r, 0, sizeof(*r));
}

void renderer_resize(int width, int height) {
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    glViewport(0, 0, width, height);
}

void renderer_draw_mesh(Renderer* r, const Mesh* mesh, Mat4 mvp) {
    glUseProgram_(r->program);
    glUniformMatrix4fv_(r->u_mvp, 1, GL_FALSE, mvp.m);

    glActiveTexture_(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (GLuint)r->texture_atlas);

    glBindVertexArray_(r->vao);
    glBindBuffer_(GL_ARRAY_BUFFER, (GLuint)r->vbo);
    glBufferData_(GL_ARRAY_BUFFER, (GLsizeiptr)(mesh->vertex_count * 8 * sizeof(float)), mesh->vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh->vertex_count);
}
