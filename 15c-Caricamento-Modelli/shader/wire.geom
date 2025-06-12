// Geometry Shader (wireframe_selector.geom)
#version 460 core

layout(triangles) in;

layout(triangle_strip, max_vertices = 16) out;
//layout(line_strip, max_vertices = 6) out;

uniform int renderMode; // 0: vertices, 1: edges, 2: faces

uniform mat4 Model2World;
uniform mat4 World2Camera;
uniform mat4 ProjectionMatrix;
uniform vec3 CameraPosition;

flat in float vSelected[]; // flat = non interpolato

out vec4 geomColor;

uniform vec2 viewport_size=vec2(1024, 768);    // da passare da CPU: (width, height)
uniform float margin = 0.1;   // in pixels


void emit_vertex(int i, vec4 color) {
    //ProjectionMatrix * World2Camera * Model2World
    gl_Position =  ProjectionMatrix * World2Camera * Model2World * gl_in[i].gl_Position;
    geomColor = color;
    EmitVertex();
}
void emit_edge(int i, int j, vec4 color) {
    vec4 p0 = ProjectionMatrix * World2Camera * Model2World * gl_in[i].gl_Position;
    vec4 p1 = ProjectionMatrix * World2Camera * Model2World * gl_in[j].gl_Position;

    geomColor = color;  // Verde
    gl_Position = p0;
    EmitVertex();
    gl_Position = p1;
    EmitVertex();
    EndPrimitive();
}



vec2 to_screen(vec4 clip) {
    vec3 ndc = clip.xyz / clip.w;
    return (ndc.xy * 0.5 + 0.5) * viewport_size;
}

vec4 to_clip(vec2 screen, float z, float w) {
    vec2 ndc = (screen / viewport_size) * 2.0 - 1.0;
    return vec4(ndc * w, z * w, w);
}

void emit_band(int i1, int i2, vec4 color) {
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
    float scala=margin * clipA.w* (vSelected[i1]+vSelected[i2]);

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

    geomColor = color; // Verde

    gl_Position = v0; EmitVertex();
    gl_Position = v1; EmitVertex();
    gl_Position = v2; EmitVertex();
    EndPrimitive();
}

void main() {
    float alfa=(vSelected[0]*vSelected[1]*vSelected[2]);
    if(alfa<0.1) alfa=0.1;
    if(alfa>0.3) alfa=0.3;
    vec4 giallo= vec4(1,1,0,alfa);
    emit_vertex(0,giallo); // giallo
    emit_vertex(1, giallo);
    emit_vertex(2, giallo);
    EndPrimitive();
    

    if (renderMode == 1) {
        // Vertices
        for (int i = 0; i < 3; ++i) {
            if (vSelected[i] > 0.5) { // solo se selezionato
            vec4 worldPos = Model2World * gl_in[i].gl_Position;
            vec4 clip = ProjectionMatrix * World2Camera * worldPos;
            vec2 screen = to_screen(clip);
            
            float size = margin * clip.w * 15.0; // dimensione modificabile
            if (size > 5.0) size = 5.0;
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

            geomColor = vec4(1, 0, 0, 1); // rosso

            gl_Position = va; EmitVertex();
            gl_Position = vb; EmitVertex();
            gl_Position = vc; EmitVertex();
            gl_Position = vd; EmitVertex();
            EndPrimitive();
            
        }
        }
    } else if (renderMode == 2) {

        if(vSelected[0]*vSelected[1]==1)
            emit_band(0, 1,vec4(1,0.8,0,(vSelected[0]*vSelected[1])));
        if(vSelected[1]*vSelected[2]==1)
            emit_band(1, 2,vec4(1,0.8,0,(vSelected[1]*vSelected[2])));
        if(vSelected[2]*vSelected[0]==1)
            emit_band(2, 0,vec4(1,0.8,0,(vSelected[2]*vSelected[0])));
    } 
}
