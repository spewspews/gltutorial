#version 330

uniform sampler2D glendatex;
uniform sampler2D kittytex;

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

void
main()
{
	vec4 glendacol = texture(glendatex, Texcoord);
	vec4 kittycol = texture(kittytex, Texcoord);
	outColor = mix(glendacol, kittycol, 0.5) * vec4(Color, 0.0);
}
