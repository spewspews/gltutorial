#version 330

uniform vec3 tricolor;

out vec4 outColor;

void
main()
{
	outColor = vec4(tricolor, 1.0);
}
