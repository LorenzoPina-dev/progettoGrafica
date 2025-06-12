#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 16) out;

uniform int renderMode; // 0: vertices, 1: edges, 2: faces

uniform mat4 Model2World;
uniform mat4 World2Camera;
uniform mat4 ProjectionMatrix;

flat in uint vObjectID[];
flat in uint vVertexID[];
// Vettori della normali ricevuti dal vertex shader

flat out uvec4 FinalId;

uniform vec2 viewport_size=vec2(1024, 768);    // da passare da CPU: (width, height)
uniform float margin = 0.7;   // in pixels

void emit_vertex(int i) {
    gl_Position =  ProjectionMatrix * World2Camera * Model2World * gl_in[i].gl_Position;
    //FinalId=uvec4(vObjectID[i]+1,vVertexID[i]+1,0,0);
    EmitVertex();
}


vec2 to_screen(vec4 clip) {
    vec3 ndc = clip.xyz / clip.w;
    return (ndc.xy * 0.5 + 0.5) * viewport_size;
}

vec4 to_clip(vec2 screen, float z, float w) {
    vec2 ndc = (screen / viewport_size) * 2.0 - 1.0;
    return vec4(ndc * w, z * w, w);
}

void emit_band(int i1,int i2) {
    vec4 worldA = Model2World * gl_in[i1].gl_Position;
    vec4 worldB = Model2World * gl_in[i2].gl_Position;

    vec4 clipA = ProjectionMatrix * World2Camera * worldA;
    vec4 clipB = ProjectionMatrix * World2Camera * worldB;
    
    vec2 screenA = to_screen(clipA);
    vec2 screenB = to_screen(clipB);

    vec2 dir =screenB - screenA;
    float len = length(dir);
    if (len < 0.001) {
        dir = vec2(1.0, 0.0); // default direction to avoid zero length
    } else {
        dir /= len;
    }


    vec2 perp = vec2(-dir.y, dir.x);
    float scala=margin * clipA.w;

    if(scala>5) scala=5;
    if(scala<2) scala=2;

    vec2 offset = perp * scala;//length(CameraPosition-(worldB.xyz+worldA.xyz)*0.5);

    vec2 a0 = screenA - offset;
    vec2 a1 = screenA + offset;
    vec2 b0 = screenB - offset;
    vec2 b1 = screenB + offset;

    float zA = clipA.z / clipA.w;
    float zB = clipB.z / clipB.w;
    float wA = clipA.w;
    float wB = clipB.w;

    vec4 v0 = to_clip(a0, zA, wA);
    vec4 v1 = to_clip(a1, zA, wA);
    vec4 v2 = to_clip(b0, zB, wB);
    vec4 v3 = to_clip(b1, zB, wB);


    //FinalId=uvec4(vObjectID[i1]+1,vVertexID[i1]+1,vVertexID[i2]+1,0);
    gl_Position = v0; EmitVertex();
    gl_Position = v1; EmitVertex();
    gl_Position = v2; EmitVertex();
    gl_Position = v3; EmitVertex();
    EndPrimitive();
}

void main() {
    
    if (renderMode == 1) {
        // Vertices
        for (int i = 0; i < 3; ++i) {
            vec4 worldPos = Model2World * gl_in[i].gl_Position;
            vec4 clip = ProjectionMatrix * World2Camera * worldPos;
            vec2 screen = to_screen(clip);
            
            float size = margin * clip.w*15.0; // dimensione modificabile
            if (size > 10.0) size = 10.0;
            if (size < 2.0) size = 2.0;

            vec2 a = screen + vec2(-size, -size);
            vec2 b = screen + vec2( size, -size);
            vec2 c = screen + vec2(-size,  size);
            vec2 d = screen + vec2( size,  size);

            float z = clip.z / clip.w;
            float w = clip.w;

            vec4 va = to_clip(a, z, w);
            vec4 vb = to_clip(b, z, w);
            vec4 vc = to_clip(c, z, w);
            vec4 vd = to_clip(d, z, w);

            FinalId=uvec4(vObjectID[i]+1,vVertexID[i]+1,0,0);

            gl_Position = va; EmitVertex();
            gl_Position = vb; EmitVertex();
            gl_Position = vc; EmitVertex();
            gl_Position = vd; EmitVertex();
            EndPrimitive();
        }
    } else if (renderMode == 2) {
        // Edges
        
        FinalId=uvec4(vObjectID[0]+1,vVertexID[0]+1,vVertexID[1]+1,0);
        emit_band(0, 1);
        FinalId=uvec4(vObjectID[1]+1,vVertexID[1]+1,vVertexID[2]+1,0);
        emit_band(1, 2);
        FinalId=uvec4(vObjectID[2]+1,vVertexID[2]+1,vVertexID[0]+1,0);
        emit_band(2, 0);
    } else if (renderMode == 3) {
        // Faces
        FinalId=uvec4(vObjectID[0]+1,vVertexID[0]+1,vVertexID[1]+1,vVertexID[2]+1);
        emit_vertex(0);
        emit_vertex(1);
        emit_vertex(2);
        EndPrimitive();
        
    }
}
