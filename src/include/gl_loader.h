#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <GL/gl.h>

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_DYNAMIC_DRAW
#define GL_DYNAMIC_DRAW 0x88E8
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_TEXTURE_2D
#define GL_TEXTURE_2D 0x0DE1
#endif
#ifndef GL_TEXTURE_WRAP_S
#define GL_TEXTURE_WRAP_S 0x2802
#endif
#ifndef GL_TEXTURE_WRAP_T
#define GL_TEXTURE_WRAP_T 0x2803
#endif
#ifndef GL_TEXTURE_MIN_FILTER
#define GL_TEXTURE_MIN_FILTER 0x2801
#endif
#ifndef GL_TEXTURE_MAG_FILTER
#define GL_TEXTURE_MAG_FILTER 0x2800
#endif
#ifndef GL_REPEAT
#define GL_REPEAT 0x2901
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_NEAREST
#define GL_NEAREST 0x2600
#endif
#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif
#ifndef GL_NEAREST_MIPMAP_NEAREST
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#endif
#ifndef GL_LINEAR_MIPMAP_LINEAR
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif
#ifndef GL_DEPTH_TEST
#define GL_DEPTH_TEST 0x0B71
#endif
#ifndef GL_CULL_FACE
#define GL_CULL_FACE 0x0B44
#endif
#ifndef GL_FRONT
#define GL_FRONT 0x0404
#endif
#ifndef GL_BACK
#define GL_BACK 0x0405
#endif
#ifndef GL_CCW
#define GL_CCW 0x0901
#endif
#ifndef GL_TRIANGLES
#define GL_TRIANGLES 0x0004
#endif

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, ptrdiff_t offset, GLsizeiptr size, const void* data);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_;
extern PFNGLGENBUFFERSPROC glGenBuffers_;
extern PFNGLBINDBUFFERPROC glBindBuffer_;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers_;
extern PFNGLBUFFERDATAPROC glBufferData_;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData_;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
extern PFNGLCREATESHADERPROC glCreateShader_;
extern PFNGLSHADERSOURCEPROC glShaderSource_;
extern PFNGLCOMPILESHADERPROC glCompileShader_;
extern PFNGLGETSHADERIVPROC glGetShaderiv_;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
extern PFNGLCREATEPROGRAMPROC glCreateProgram_;
extern PFNGLATTACHSHADERPROC glAttachShader_;
extern PFNGLLINKPROGRAMPROC glLinkProgram_;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv_;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_;
extern PFNGLUSEPROGRAMPROC glUseProgram_;
extern PFNGLDELETESHADERPROC glDeleteShader_;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram_;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_;
extern PFNGLUNIFORM1IPROC glUniform1i_;
extern PFNGLUNIFORM3FVPROC glUniform3fv_;
extern PFNGLACTIVETEXTUREPROC glActiveTexture_;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap_;

bool gl_loader_init(void);
