#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Iluminación
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 emissionColor;

// Posiciones de los spotlights
uniform vec3 lightPos1Left;
uniform vec3 lightPos1Right;
uniform vec3 lightPos2Left;
uniform vec3 lightPos2Right;

// Direcciones individuales de cada spotlight
uniform vec3 lightDir1Left;
uniform vec3 lightDir1Right;
uniform vec3 lightDir2Left;
uniform vec3 lightDir2Right;

// Ángulos de corte
uniform float cutoff;       // Ángulo interno (cos)
uniform float outerCutoff;  // Ángulo externo (cos)

// Material
uniform bool hasTexture;
uniform vec3 materialColor;
uniform sampler2D texture_diffuse1;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- Cálculo para cada foco ---

    // --- Carro 1 - Izquierdo ---
    vec3 lightToFrag1Left = normalize(lightPos1Left - FragPos);
    float theta1Left = dot(lightToFrag1Left, normalize(-lightDir1Left));
    float intensity1Left = clamp((theta1Left - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float diff1Left = max(dot(norm, lightToFrag1Left), 0.0);
    vec3 reflectDir1Left = reflect(-lightToFrag1Left, norm);
    float spec1Left = pow(max(dot(viewDir, reflectDir1Left), 0.0), 32);

    // --- Carro 1 - Derecho ---
    vec3 lightToFrag1Right = normalize(lightPos1Right - FragPos);
    float theta1Right = dot(lightToFrag1Right, normalize(-lightDir1Right));
    float intensity1Right = clamp((theta1Right - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float diff1Right = max(dot(norm, lightToFrag1Right), 0.0);
    vec3 reflectDir1Right = reflect(-lightToFrag1Right, norm);
    float spec1Right = pow(max(dot(viewDir, reflectDir1Right), 0.0), 32);

    // --- Carro 2 - Izquierdo ---
    vec3 lightToFrag2Left = normalize(lightPos2Left - FragPos);
    float theta2Left = dot(lightToFrag2Left, normalize(-lightDir2Left));
    float intensity2Left = clamp((theta2Left - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float diff2Left = max(dot(norm, lightToFrag2Left), 0.0);
    vec3 reflectDir2Left = reflect(-lightToFrag2Left, norm);
    float spec2Left = pow(max(dot(viewDir, reflectDir2Left), 0.0), 32);

    // --- Carro 2 - Derecho ---
    vec3 lightToFrag2Right = normalize(lightPos2Right - FragPos);
    float theta2Right = dot(lightToFrag2Right, normalize(-lightDir2Right));
    float intensity2Right = clamp((theta2Right - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float diff2Right = max(dot(norm, lightToFrag2Right), 0.0);
    vec3 reflectDir2Right = reflect(-lightToFrag2Right, norm);
    float spec2Right = pow(max(dot(viewDir, reflectDir2Right), 0.0), 32);

    // Componentes de luz
    float specularStrength = 0.2;

    vec3 diffuse = lightColor * (
        diff1Left * intensity1Left +
        diff1Right * intensity1Right +
        diff2Left * intensity2Left +
        diff2Right * intensity2Right
    );

    vec3 specular = lightColor * specularStrength * (
        spec1Left * intensity1Left +
        spec1Right * intensity1Right +
        spec2Left * intensity2Left +
        spec2Right * intensity2Right
    );

    // Combinar todas las luces
    vec3 lighting = diffuse + specular + emissionColor;

    // Color base del material
    vec3 baseColor = hasTexture ? texture(texture_diffuse1, TexCoords).rgb : materialColor;

    // Color final
    FragColor = vec4(baseColor * lighting, 1.0);
}
