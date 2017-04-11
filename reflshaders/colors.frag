#version 330

uniform sampler2D glendatex;

in vec3 Color;
in vec2 Texpos;

out vec4 outColor;

void
main()
{
	if(Texpos.y < 0.5)
		outColor = texture(glendatex, Texpos) * vec4(Color, 0.0);
	else
		outColor = texture(glendatex, vec2(Texpos.x, 1-Texpos.y));
}
