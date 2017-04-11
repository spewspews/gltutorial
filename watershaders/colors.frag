#version 330

uniform sampler2D glendatex;
uniform float water;

in vec3 Color;
in vec2 Texpos;

out vec4 outColor;

#define pi 3.14159265358979323846

void
main()
{
	if(Texpos.y < 0.5)
		outColor = texture(glendatex, Texpos);
	else {
		float sampleOff = sin((Texpos.y-0.5)*50+water)*0.2 - 0.2*sin(water);
		outColor = texture(glendatex, vec2(Texpos.x + sampleOff, Texpos.y));
	}
}
