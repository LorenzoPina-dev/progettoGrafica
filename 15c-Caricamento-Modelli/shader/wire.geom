// Geometry Shader (wireframe_selector.geom)
#version 460 core

layout(triangles) in;

layout(triangle_strip, max_vertices = 16) out;

uniform int renderMode; // 0: vertices, 1: edges, 2: faces

uniform mat4 Model2World;
uniform mat4 World2Camera;
uniform mat4 ProjectionMatrix;
uniform vec3 CameraPosition;

// ID vertice passati dal vertex shader (non interpolati)
flat in float vSelected[]; // flat = non interpolato

out vec4 geomColor;

uniform vec2 viewport_size=vec2(1024, 768);    // da passare da CPU: (width, height)
uniform float margin = 0.1;   // in pixels

// Trasforma da clip-space a coordinate schermo in pixel
vec2 to_screen(vec4 clip) {
    vec3 ndc = clip.xyz / clip.w;
    return (ndc.xy * 0.5 + 0.5) * viewport_size;
}

// Trasforma coordinate schermo + profondità in clip-space
vec4 to_clip(vec2 screen, float z, float w) {
    vec2 ndc = (screen / viewport_size) * 2.0 - 1.0;
    return vec4(ndc * w, z * w, w);
}

// Trasforma un indice di input in posizione clip-space ed emette un vertice
void emit_vertex(int i) {
    gl_Position =  ProjectionMatrix * World2Camera * Model2World * gl_in[i].gl_Position;
    EmitVertex();
}

void emit_band(int i1, int i2, vec4 color) {
    // Posizioni nel mondo
    vec4 worldA = Model2World * gl_in[i1].gl_Position;
    vec4 worldB = Model2World * gl_in[i2].gl_Position;

    // Proiezione in clip-space
    vec4 clipA = ProjectionMatrix * World2Camera * worldA;
    vec4 clipB = ProjectionMatrix * World2Camera * worldB;
    
    // Coordinate schermo in pixel
    vec2 screenA = to_screen(clipA);
    vec2 screenB = to_screen(clipB);

    // Direzione dello spigolo
    vec2 dir =screenB - screenA;
    float len = length(dir);
    if (len < 0.001) {
        dir = vec2(1.0, 0.0); // default direction to avoid zero length
    } else {
        dir /= len;
    }


    // Vettore perpendicolare per generare lo spessore
    vec2 perp = vec2(-dir.y, dir.x);
    float scala=margin * clipA.w* (vSelected[i1]+vSelected[i2]);

    if(scala>5) scala=5;
    if(scala<2) scala=2;

    vec2 offset = perp * scala;

    // Costruzione dei 4 vertici del rettangolo sullo schermo
    vec2 a0 = screenA - offset;
    vec2 a1 = screenA + offset;
    vec2 b0 = screenB - offset;
    vec2 b1 = screenB + offset;

    float zA = clipA.z / clipA.w;
    float zB = clipB.z / clipB.w;
    float wA = clipA.w;
    float wB = clipB.w;

    // Conversione dei vertici in clip-space
    vec4 v0 = to_clip(a0, zA, wA);
    vec4 v1 = to_clip(a1, zA, wA);
    vec4 v2 = to_clip(b0, zB, wB);
    vec4 v3 = to_clip(b1, zB, wB);

    //setto colore
    geomColor = color; // Verde

    // Emetti i 4 vertici come un quad (due triangoli)
    gl_Position = v0; EmitVertex();
    gl_Position = v1; EmitVertex();
    gl_Position = v2; EmitVertex();
    gl_Position = v3; EmitVertex();
    EndPrimitive();
}

void main() {
    float alfa=(vSelected[0]*vSelected[1]*vSelected[2]);
    if(alfa<0.1) alfa=0.1;
    if(alfa>0.3) alfa=0.3;
    geomColor= vec4(1,1,0,alfa);
    emit_vertex(0); // giallo
    emit_vertex(1);
    emit_vertex(2);
    EndPrimitive();
    

    if (renderMode == 1) {
        // Rendering dei vertici come piccoli quad (punti)
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
        // Rendering degli spigoli come bande rettangolari
        if(vSelected[0]*vSelected[1]==1)
            emit_band(0, 1,vec4(1,0.8,0,(vSelected[0]*vSelected[1])));
        if(vSelected[1]*vSelected[2]==1)
            emit_band(1, 2,vec4(1,0.8,0,(vSelected[1]*vSelected[2])));
        if(vSelected[2]*vSelected[0]==1)
            emit_band(2, 0,vec4(1,0.8,0,(vSelected[2]*vSelected[0])));
    } 
}
