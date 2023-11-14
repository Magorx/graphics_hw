#version 450

#extension GL_GOOGLE_include_directive : require

#include "common.h"
#include "unpack_attributes.h"

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

layout(location = 0) in VS_OUT {
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} vsout[];

layout(location = 0) out VS_OUT {
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} gout;

layout(push_constant) uniform params_t {
    mat4 mProjView;
    mat4 mModel;
} params;

layout (binding = 0, set = 0) uniform AppData {
    UniformParams global_params;
};

void emmit_vertex(int i) {
    gout.texCoord = vsout[i].texCoord;
    gout.wNorm = vsout[i].wNorm;
    gout.wPos = vsout[i].wPos;
    gout.wTangent = vsout[i].wTangent;
    gl_Position = params.mProjView * vec4(vsout[i].wPos, 1.0);
    EmitVertex();
}

void main () {
    vec3 cpos = vec3(0);
    vec3 cnorm = vec3(0);

    for (int i = 0; i < gl_in.length(); ++i) {
        emmit_vertex(i);

        cpos = cpos + vsout[i].wPos;
        cnorm = cnorm + vsout[i].wNorm;
    }
    EndPrimitive();

    cpos = cpos / 3;
    cnorm = cnorm / 3;
    vec4 top_pos = params.mProjView * vec4(cpos - cnorm * 0.1 * sin(global_params.time), 1.0);

    emmit_vertex(0);
    emmit_vertex(1);

    gout.texCoord = vsout[0].texCoord;
    gout.wNorm = cnorm;
    gout.wPos = cpos;
    gout.wTangent = vsout[1].wTangent;
    gl_Position = top_pos;
    EmitVertex();

    emmit_vertex(2);
    emmit_vertex(0);

    EndPrimitive();
}