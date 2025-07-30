#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Iluminación
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 emissionColor;

// Parámetros de los spotlights
uniform vec3 lightPos1Left;  // Foco izquierdo del carro 1
uniform vec3 lightPos1Right;  // Foco derecho del carro 1
uniform vec3 lightPos2Left;  // Foco izquierdo del carro 2
uniform vec3 lightPos2Right;  // Foco derecho del carro 2
uniform vec3 lightDirection;  // Dirección hacia la que apunta el foco
uniform float cutoff;  // Ángulo de corte
uniform float outerCutoff;  // Ángulo de atenuación exterior

// Material
uniform bool hasTexture;
uniform vec3 materialColor;
uniform sampler2D texture_diffuse1;

void main()
{
    // --- Cálculo de iluminación ---
    vec3 norm = normalize(Normal);

    // Cálculo de la dirección de la luz para cada foco
    vec3 lightDir1Left = normalize(lightPos1Left - FragPos);
    vec3 lightDir1Right = normalize(lightPos1Right - FragPos);
    vec3 lightDir2Left = normalize(lightPos2Left - FragPos);
    vec3 lightDir2Right = normalize(lightPos2Right - FragPos);

    // Cálculo del ángulo de incidencia para cada foco
    float theta1Left = dot(lightDir1Left, normalize(-lightDirection)); 
    float theta1Right = dot(lightDir1Right, normalize(-lightDirection));
    float theta2Left = dot(lightDir2Left, normalize(-lightDirection)); 
    float theta2Right = dot(lightDir2Right, normalize(-lightDirection));

    // Atenuación para cada foco con el corte de ángulo
    float intensity1Left = clamp((theta1Left - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float intensity1Right = clamp((theta1Right - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float intensity2Left = clamp((theta2Left - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);
    float intensity2Right = clamp((theta2Right - outerCutoff) / (cutoff - outerCutoff), 0.0, 1.0);

    // Iluminación difusa
    float diff1Left = max(dot(norm, lightDir1Left), 0.0);
    float diff1Right = max(dot(norm, lightDir1Right), 0.0);
    float diff2Left = max(dot(norm, lightDir2Left), 0.0);
    float diff2Right = max(dot(norm, lightDir2Right), 0.0);

    // Iluminación especular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir1Left = reflect(-lightDir1Left, norm);
    vec3 reflectDir1Right = reflect(-lightDir1Right, norm);
    vec3 reflectDir2Left = reflect(-lightDir2Left, norm);
    vec3 reflectDir2Right = reflect(-lightDir2Right, norm);

    float spec1Left = pow(max(dot(viewDir, reflectDir1Left), 0.0), 32);
    float spec1Right = pow(max(dot(viewDir, reflectDir1Right), 0.0), 32);
    float spec2Left = pow(max(dot(viewDir, reflectDir2Left), 0.0), 32);
    float spec2Right = pow(max(dot(viewDir, reflectDir2Right), 0.0), 32);

    // Calcular la luz para cada foco
    vec3 diffuse1Left = diff1Left * lightColor * intensity1Left;
    vec3 diffuse1Right = diff1Right * lightColor * intensity1Right;
    vec3 diffuse2Left = diff2Left * lightColor * intensity2Left;
    vec3 diffuse2Right = diff2Right * lightColor * intensity2Right;

    vec3 specular1Left = specularStrength * spec1Left * lightColor;
    vec3 specular1Right = specularStrength * spec1Right * lightColor;
    vec3 specular2Left = specularStrength * spec2Left * lightColor;
    vec3 specular2Right = specularStrength * spec2Right * lightColor;

    // Combina las cuatro luces
    vec3 lighting = (diffuse1Left + diffuse1Right + diffuse2Left + diffuse2Right +
                     specular1Left + specular1Right + specular2Left + specular2Right) + emissionColor;

    // Selección de color base (con o sin textura)
    vec3 baseColor = hasTexture ? texture(texture_diffuse1, TexCoords).rgb : materialColor;

    // Aplicar iluminación
    vec3 finalColor = baseColor * lighting;
    FragColor = vec4(finalColor, 1.0);
}
