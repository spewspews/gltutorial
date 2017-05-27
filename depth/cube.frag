#version 330 core
uniform sampler2D glendatex;
uniform sampler2D kittytex;
uniform float blend;
uniform float haze;
in vec2 Texpos;
out vec4 outColor;

void
main()
{
	vec4 glendacol = texture(glendatex, Texpos);
	vec4 kittycol = texture(kittytex, Texpos);
	outColor = vec4(haze, haze, haze, 1.0) * mix(glendacol, kittycol, blend);
}
