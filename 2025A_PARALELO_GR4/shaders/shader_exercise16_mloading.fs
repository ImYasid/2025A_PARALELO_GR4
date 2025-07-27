#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Iluminación
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 emissionColor;

// Material
uniform bool hasTexture;           // Nuevo: indica si hay textura
uniform vec3 materialColor;        // Nuevo: color gris claro (0.7, 0.7, 0.7) cuando no hay textura
uniform sampler2D texture_diffuse1; // Textura difusa

void main()
{
    // --- Cálculo de iluminación (igual que antes) ---
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine lighting
    vec3 lighting = ambient + diffuse + specular + emissionColor;

    // --- Selección de color base (textura o color gris) ---
    vec3 baseColor;
    if (hasTexture) {
        baseColor = texture(texture_diffuse1, TexCoords).rgb;
    } else {
        baseColor = materialColor; // Gris claro (0.7, 0.7, 0.7)
    }

    // Aplicar iluminación
    vec3 finalColor = baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}
