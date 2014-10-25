uniform mat4 ng_ProjMatrix;
uniform mat4 ng_NodeMatrix;
uniform mat4 ng_FullMatrix;
uniform vec3 ngGo_Direction;
attribute vec2 position;
attribute vec2 uv;
varying vec2 frag_uv;
varying mediump vec3 H;

const mediump vec3 LightDirection = vec3(1.0, 0.3, 0.2);
const float LightDistance = 20.0;
const float SpecularContribution = 0.4;
const float AmbientContribution = 0.1;
const float DiffuseContribution = 1.0 - AmbientContribution - SpecularContribution;

void main()
{
    frag_uv = uv;

    mediump vec3 P = vec3(ng_ProjMatrix * vec4(position, 0.0, 0.0));
    mediump vec3 L = normalize(ngGo_Direction - P);
    mediump vec3 V = normalize(vec3(1.0, 0.0, 0.0) - P);

    H = L + V;

    gl_Position = ng_FullMatrix * vec4(position, 0.0, 1.0);
}
