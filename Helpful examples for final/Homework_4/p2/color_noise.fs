#version 410 core                                                 

uniform sampler2D texture_background; //this is colormap
uniform sampler2D texture_foreground; //this is noisemap

in vec2 pass_TexCoord; //this is the texture coord
in vec4 pass_Color;
out vec4 color;

uniform int texture_blend;

void main(void)                                                   
	
{
	vec2 noisevec;
	
	noisevec = normalize(texture(texture_foreground, pass_TexCoord).xy);
	
	noisevec = (noisevec * 2.0 - 1.0 ) * 0.02;

	color = texture(texture_background, pass_TexCoord + noisevec) ;
}