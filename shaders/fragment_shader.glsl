// fragment_shader.glsl
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float Height;

out vec4 FragColor;

uniform sampler2D terrainTexture;

void main() {
    // Height-based coloring
    vec3 baseColor;
    if (Height < 10.0) {
        baseColor = vec3(0.2, 0.5, 0.2);  // Low ground (green)
    } else if (Height < 50.0) {
        baseColor = vec3(0.5, 0.5, 0.2);  // Hills (yellow-ish)
    } else {
        baseColor = mix(vec3(0.5, 0.5, 0.5), vec3(1.0), (Height - 50.0) / 100.0);  // Mountains (gray to white)
    }

    // Basic lighting
    vec3 lightDir = normalize(vec3(-0.2, -1.0, -0.3));
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Combine lighting with height-based color and texture
    vec3 texColor = texture(terrainTexture, TexCoords).rgb;
    vec3 finalColor = baseColor * texColor * (0.3 + 0.7 * diffuse);  // Ambient + diffuse lighting
    
    FragColor = vec4(finalColor, 1.0);
}
