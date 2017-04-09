#version 330

uniform sampler2D glendatex;
uniform sampler2D kittytex;
uniform float blend;

in vec3 Color;
in vec2 Texpos;

out vec4 outColor;

void
main()
{
	vec4 glendacol = texture(glendatex, Texpos);
	vec4 kittycol = texture(kittytex, Texpos);
	outColor = mix(glendacol, kittycol, blend) * vec4(Color, 0.0);
}
