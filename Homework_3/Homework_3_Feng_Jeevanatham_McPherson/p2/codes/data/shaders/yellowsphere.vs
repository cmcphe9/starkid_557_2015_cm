#Jiale Feng 2015-10-14
#version 330 core

// The vertex buffer input                                    
in vec3 in_Color; 
in vec3 in_Position; 
in vec3 in_Normal;  

// Transformations for the projections
uniform mat4 projectionMatrixBox;
uniform mat4 viewMatrixBox;
uniform mat4 modelMatrixBox;
uniform mat4 inverseViewMatrix;

// The material parameters 
uniform vec3 diffuse_color;                                        
uniform vec3 ambient_color;                                         
uniform vec3 specular_color;   

// Position of the light source
uniform vec4 light_position;   
uniform vec4 light_position1;
// New parameters for the spotlight
uniform float cone_angle;
uniform vec3 cone_direction;

// The intensity values for the reflection equations
uniform float diffuse_intensity;
uniform float diffuse_intensity1;
uniform float ambient_intensity;
uniform float ambient_intensity1;
uniform float specular_intensity;
uniform float specular_intensity1;
uniform float shininess;    
uniform float attenuationCoefficient;
uniform float attenuationCoefficient1;
                                              



// The output color
out vec3 pass_Color;                                            
                                                                 
                                                             
                                                                                                                               
void main(void)                                                 
{                                                               
	vec3 normal = normalize(in_Normal);                                                                   
    vec4 transformedNormal = normalize(transpose(inverse( modelMatrixBox)) * vec4( normal, 1.0 ));
    
    vec4 surfacePostion = modelMatrixBox * vec4(in_Position, 1.0);
                                                                                                       
    vec4 surface_to_light =   normalize( light_position -  surfacePostion );                      
                                                                                                            
    // Diffuse color                                                                                          
    float diffuse_coefficient = max( dot(transformedNormal, surface_to_light), 0.0);
    vec3 out_diffuse_color = diffuse_color  * diffuse_coefficient * diffuse_intensity;                        
                                                                                                              
    // Ambient color                                                                                         
    vec3 out_ambient_color = vec3(ambient_color) * ambient_intensity;                                        
                                                                                                             
    // Specular color                                                                                        
    vec3 incidenceVector = -surface_to_light.xyz;
    vec3 reflectionVector = reflect(incidenceVector, normal.xyz);
    
    vec3 cameraPosition = vec3( inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePostion.xyz);
    
    float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
    float specular_coefficient = pow(cosAngle, shininess);                                                     
    vec3 out_specular_color = specular_color * specular_coefficient * specular_intensity;                    
  
	
	//attenuation
    float distanceToLight = length(light_position.xyz - surfacePostion.xyz);
    float attenuation = 1.0 / (1.0 + attenuationCoefficient * pow(distanceToLight, 2));
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////        
    // Spotlight
    // 1. Normalize the cone direction
    vec3 cone_direction_norm = normalize(cone_direction); 
    
    // 2. Calculate the ray direction. We already calculated the surface to light direction.
    // 	  All what we need to do is to inverse this value
    vec3 ray_direction = -surface_to_light.xyz;   
    
    // 3. Calculate the angle between light and surface using the dot product again. 
    //    To simplify our understanding, we use the degrees
    float light_to_surface_angle = degrees(acos(dot(ray_direction, cone_direction_norm))) ; 
    
    // 4. Last, we compare the angle with the current direction and 
    
    //    reduce the attenuation to 0.0 if the light is outside the angle.
    float spotEffect = 1.0;
    
	if(light_to_surface_angle > cone_angle){
  		attenuation = 0.0;
	}
    else{
        spotEffect = 1 - smoothstep(cone_angle - 4.3, cone_angle, light_to_surface_angle);
    }
	
	
	// Calculate the linear color
	vec3 linearColor = out_ambient_color  + spotEffect * attenuation * ( out_diffuse_color + out_specular_color);
   
    
    //Calculate linearColor for second lightsource
    vec3 normal1 = normalize(in_Normal);
    vec4 transformedNormal1 = normalize(transpose(inverse( modelMatrixBox)) * vec4( normal1, 1.0 ));
    
    vec4 surfacePostion1 = modelMatrixBox * vec4(in_Position, 1.0);
    vec4 surface_to_light1 =   normalize( light_position1 -  surfacePostion1 );
    if(light_position1.w == 0.0){
        surface_to_light1 =   normalize( light_position1);
    }
    
    // Diffuse color
    float diffuse_coefficient1 = max( dot(transformedNormal1, surface_to_light1), 0.0);
    vec3 out_diffuse_color1 = diffuse_color  * diffuse_coefficient1 * diffuse_intensity1;
    
    // Ambient color
    vec3 out_ambient_color1 = vec3(ambient_color) * ambient_intensity1;
    
    // Specular color
    vec3 incidenceVector1 = -surface_to_light1.xyz;
    vec3 reflectionVector1 = reflect(incidenceVector1, normal1);
    vec3 cameraPosition1 = vec3( inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
    vec3 surfaceToCamera1 = normalize(cameraPosition1 - surfacePostion1.xyz);
    float cosAngle1 = max( dot(surfaceToCamera1, reflectionVector1), 0.0);
    float specular_coefficient1 = pow(cosAngle1, shininess);
    vec3 out_specular_color1 = specular_color * specular_coefficient1 * specular_intensity1;
    
    
    //attenuation
    float distanceToLight1 = length(light_position1.xyz - surfacePostion1.xyz);
    float attenuation1 = 1.0 / (1.0 + attenuationCoefficient1 * pow(distanceToLight1, 2));
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    // Directional light
    //
    if(light_position1.w == 0.0) {
        // this is a directional light.
        
        // 1. the values that we store as light position is our light direction.
        vec3 light_direction1 = normalize(light_position1.xyz);
        
        // 2. We check the angle of our light to make sure that only parts towards our light get illuminated
        float light_to_surface_angle1 = dot(light_direction1, transformedNormal1.xyz);
        
        // 3. Check the angle, if the angle is smaller than 0.0, the surface is not directed towards the light.
        if(light_to_surface_angle1 > 0.0)attenuation1 = 1.0;
        else attenuation1 = 0.0;
    }
    
    
    // Calculate the linear color
    linearColor += out_ambient_color1  + attenuation1 * ( out_diffuse_color1 + out_specular_color1);
    
	// Gamma correction
	vec3 gamma = vec3(1.0/2.2);
	vec3 finalColor = pow(linearColor, gamma);
	
	// Pass the color
	pass_Color =  finalColor;
	
	// Passes the projected position to the fragment shader / rasterization process. 
    gl_Position = projectionMatrixBox * viewMatrixBox * modelMatrixBox * vec4(in_Position, 1.0);                                                                                                                   
                          
}                                                                                                            