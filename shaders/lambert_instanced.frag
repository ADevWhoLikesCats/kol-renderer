#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vColor;

uniform vec3  uLightDir;
uniform vec3  uLightColor;
uniform vec3  uAmbient;
uniform sampler2D uAlbedo;
uniform bool  uUseTexture;
uniform vec2  uUVScale;

out vec4 FragColor;

void main() {
    vec2 uv = vUV * uUVScale;
    vec3 albedo = uUseTexture ? texture(uAlbedo, uv).rgb : vColor;

    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    float ndl = max(dot(N, L), 0.0);

    vec3 color = albedo * (uAmbient + uLightColor * ndl);
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}
