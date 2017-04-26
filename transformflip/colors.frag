#version 330 core
uniform sampler2D glendatex;
uniform sampler2D kittytex;
in vec2 Texpos;
out vec4 outColor;
void main()
{
	vec4 glendacol = texture(glendatex, Texpos);
	vec4 kittycol = texture(kittytex, Texpos);
	outColor = mix(glendacol, kittycol, 0.5);
}
