#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 lightPosEye1; 
in vec3 lightPosEye2; 
in vec3 lightPosEye3; 


out vec4 fColor;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform vec3 pointLight1;
uniform vec3 pointLightColor1;
uniform vec3 pointLight2;
uniform vec3 pointLightColor2;
uniform vec3 pointLight3;
uniform vec3 pointLightColor3;

// Alpha threshold for transparency
uniform float alphaThreshold = 0.1f;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

void computeLightComponents()
{
    vec3 cameraPosEye = vec3(0.0f); // in eye coordinates, the viewer is situated at the origin

    // transform normal
    vec3 normalEye = normalize(fNormal);

    // compute light direction
    vec3 lightDirN = normalize(lightDir);

    // compute view direction
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    // compute ambient light
    ambient = ambientStrength * lightColor;

    // compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    // compute specular light
    vec3 reflection = normalize(lightDirN+viewDirN);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

float computeFog() 
{
    float fogDensity = 0.05f;
    float fragmentDistance = length(fPosEye); 
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
    return clamp(fogFactor, 0.0f, 1.0f); 
}
float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}
float constant = 1.0f;
float linear = 0.14f;
float quadratic = 0.07f;
void computePointLight(vec3 lPE, vec3 pointColor){
	//transform normal
	vec3 normalEye = normalize(fNormal);

	//compute light direction
	vec3 lightDirN = normalize(lPE - fPosEye.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(-fPosEye.xyz);

	vec3 halfVector = normalize(lightDirN + viewDirN);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);

	//compute distance to light
	float dist = length(lPE - fPosEye.xyz);

	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


	vec3 ambient1 = att * ambientStrength * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuse1 = att * max(dot(normalEye, lightDirN), 0.0f) * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specular1 = att * specularStrength * specCoeff * pointColor * texture(specularTexture, fTexCoords).rgb;

	ambient += ambient1;
	diffuse += diffuse1;
	specular += specular1;
}

void main() 
{
    // Compute lighting components
    computeLightComponents();

    vec3 baseColor = vec3(0.9f, 0.35f, 0.0f); // orange

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

   computePointLight(lightPosEye1, pointLightColor1);
computePointLight(lightPosEye2, pointLightColor2);
computePointLight(lightPosEye3, pointLightColor3);

float shadow=computeShadow();


    vec3 color = min((ambient +(1.0f-shadow)* diffuse) +(1.0f-shadow)* specular, 1.0f);
   

    // Compute fog blending
    float fogFactor = computeFog(); 
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    // Alpha discard logic
    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
    if (colorFromTexture.a < 0.1)
        discard;

    // Output final color
    fColor = fogColor*(1-fogFactor)+vec4(color,1.0f)*fogFactor;
}
