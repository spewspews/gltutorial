#version 330

uniform sampler2D tex;

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

void
main()
{
	outColor = texture(tex, Texcoord) * vec4(Color, 1.0);
}
