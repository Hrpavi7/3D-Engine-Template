#include "gl_loader.h"

#include <string.h>

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_;
PFNGLGENBUFFERSPROC glGenBuffers_;
PFNGLBINDBUFFERPROC glBindBuffer_;
PFNGLDELETEBUFFERSPROC glDeleteBuffers_;
PFNGLBUFFERDATAPROC glBufferData_;
PFNGLBUFFERSUBDATAPROC glBufferSubData_;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
PFNGLCREATESHADERPROC glCreateShader_;
PFNGLSHADERSOURCEPROC glShaderSource_;
PFNGLCOMPILESHADERPROC glCompileShader_;
PFNGLGETSHADERIVPROC glGetShaderiv_;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
PFNGLCREATEPROGRAMPROC glCreateProgram_;
PFNGLATTACHSHADERPROC glAttachShader_;
PFNGLLINKPROGRAMPROC glLinkProgram_;
PFNGLGETPROGRAMIVPROC glGetProgramiv_;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_;
PFNGLUSEPROGRAMPROC glUseProgram_;
PFNGLDELETESHADERPROC glDeleteShader_;
PFNGLDELETEPROGRAMPROC glDeleteProgram_;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_;
PFNGLUNIFORM1IPROC glUniform1i_;
PFNGLUNIFORM3FVPROC glUniform3fv_;
PFNGLACTIVETEXTUREPROC glActiveTexture_;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap_;

static void* gl_get_proc(const char* name) {
    void* p = (void*)wglGetProcAddress(name);
    if (p) return p;
    HMODULE opengl32 = GetModuleHandleA("opengl32.dll");
    if (!opengl32) return NULL;
    return (void*)GetProcAddress(opengl32, name);
}

static bool load_one(void** out, const char* name) {
    *out = gl_get_proc(name);
    return *out != NULL;
}

bool gl_loader_init(void) {
    bool ok = true;

    ok &= load_one((void**)&glGenVertexArrays_, "glGenVertexArrays");
    ok &= load_one((void**)&glBindVertexArray_, "glBindVertexArray");
    ok &= load_one((void**)&glDeleteVertexArrays_, "glDeleteVertexArrays");
    ok &= load_one((void**)&glGenBuffers_, "glGenBuffers");
    ok &= load_one((void**)&glBindBuffer_, "glBindBuffer");
    ok &= load_one((void**)&glDeleteBuffers_, "glDeleteBuffers");
    ok &= load_one((void**)&glBufferData_, "glBufferData");
    ok &= load_one((void**)&glBufferSubData_, "glBufferSubData");
    ok &= load_one((void**)&glEnableVertexAttribArray_, "glEnableVertexAttribArray");
    ok &= load_one((void**)&glVertexAttribPointer_, "glVertexAttribPointer");
    ok &= load_one((void**)&glCreateShader_, "glCreateShader");
    ok &= load_one((void**)&glShaderSource_, "glShaderSource");
    ok &= load_one((void**)&glCompileShader_, "glCompileShader");
    ok &= load_one((void**)&glGetShaderiv_, "glGetShaderiv");
    ok &= load_one((void**)&glGetShaderInfoLog_, "glGetShaderInfoLog");
    ok &= load_one((void**)&glCreateProgram_, "glCreateProgram");
    ok &= load_one((void**)&glAttachShader_, "glAttachShader");
    ok &= load_one((void**)&glLinkProgram_, "glLinkProgram");
    ok &= load_one((void**)&glGetProgramiv_, "glGetProgramiv");
    ok &= load_one((void**)&glGetProgramInfoLog_, "glGetProgramInfoLog");
    ok &= load_one((void**)&glUseProgram_, "glUseProgram");
    ok &= load_one((void**)&glDeleteShader_, "glDeleteShader");
    ok &= load_one((void**)&glDeleteProgram_, "glDeleteProgram");
    ok &= load_one((void**)&glGetUniformLocation_, "glGetUniformLocation");
    ok &= load_one((void**)&glUniformMatrix4fv_, "glUniformMatrix4fv");
    ok &= load_one((void**)&glUniform1i_, "glUniform1i");
    ok &= load_one((void**)&glUniform3fv_, "glUniform3fv");
    ok &= load_one((void**)&glActiveTexture_, "glActiveTexture");
    ok &= load_one((void**)&glGenerateMipmap_, "glGenerateMipmap");

    return ok;
}
