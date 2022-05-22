#version 330

uniform vec3 LightColor;
uniform sampler2D gSampler;
uniform vec3 CamPos; //pozycja kamery
uniform vec3 CamDir; //kierunek kamery
uniform float LightAmbient;
uniform int ShadingMode;

smooth in vec4 kolorek;
in vec3 vNormal;
in vec2 texCoord;

in vec3 Position;
in vec4 pos;
in vec3 Nurm;

in float fogCoord;
uniform float FogDensity;
vec3 FogColor = vec3(0.2,0.2,0.2);

const int MAX_DISTANCE = 25;

out vec4 outputColor;
vec4 vTexColor;

void main()
{

	float LightDistance = length(Position);

	//vec3 lightPos = vec3(4.0f);
    //vec3 lightColor = vec3(0.75);
    //vec3 LightDir = lightPos - Nurm;
    //vec3 LightDir = vec3(1.0f, 0.0f, 1.0f);
	//vec3 norm = normalize(vNormal);

	float FogFactor = 1.0 - clamp(exp(-FogDensity*fogCoord),0.0,1.0);

	vec3 nNormal = normalize(Nurm);
    vec3 nVectorToLight = normalize(Position-CamPos); 
    //"angle" between direction of object to light, and the light's direction
    float angle = dot(CamPos-CamDir, nVectorToLight);
	//apply an attenuation factor
    float dist = 1 - (LightDistance / MAX_DISTANCE);
 
    //angle of surface normal and direction of object to light
    float dir = dot(nNormal, nVectorToLight) * dist;
 
    //apply ambient light
    float light = max(dir, LightAmbient);

	if (angle < 0.7)
    {
        light = 0.01f;
    }


	vTexColor = texture2D(gSampler, texCoord);	

	//float LightDiffuse = max(0.0,dot(normalize(vNormal),-LightDir));

	//outputColor = vTexColor*vec4(LightColor*(LightAmbient/7+(5/(exp(LightDistance)))),1.0f);
	//outputColor = vTexColor*vec4(LightColor*(dir),1.0f);
	if (ShadingMode==1) {outputColor = mix(kolorek*vTexColor*vec4(LightColor*(max(LightAmbient,(10/(LightDistance*LightDistance*LightDistance)))),1.0f), vec4(FogColor,1.0), FogFactor);}
	else {outputColor = kolorek*(1.0,1.0,1.0,vTexColor.r);}
	//outputColor = kolorek*vTexColor*vec4(LightColor*(LightAmbient/2+LightDiffuse*attenuation),1.0f);	
	//outputColor = kolorek*vTexColor*vec4(LightColor*(LightAmbient+LightDiffuse),1.0f);	
	//outputColor = kolorek*vec4(LightColor*(LightAmbient+LightDiffuse),1.0f);	
}

/*uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 LightPosition;
float constant;
float linear;
float quadratic;
in vec3 FragPos;
uniform int ShadingMode;
uniform sampler2D gSampler;

uniform float LightAmbient;

smooth in vec4 kolorek;
in vec3 vNormal;
in vec2 texCoord;

in vec3 Pozycja;

out vec4 outputColor;
vec4 vTexColor;

void main()
{
	  constant =1; linear=0.7; quadratic=0.3;
	  vTexColor = texture2D(gSampler, texCoord);	
	  vec3 Poz = normalize(Pozycja);

	  vec3 LightDir = normalize(LightPosition-Pozycja);
	  vec3 norm = normalize(vNormal);

	  float LightDiffuse = max(dot(norm,LightDir),0.0);
	  
	  float distance = length(LightPosition-Pozycja);

	  float att= 10.0f/(constant+linear*distance+quadratic*distance*distance);

	  outputColor = kolorek*vTexColor*vec4(LightColor*(LightAmbient*2+LightDiffuse*att),1.0);	

}*/