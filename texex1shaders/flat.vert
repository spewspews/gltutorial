#version 330

in vec2 texpos;
in vec2 position;
in vec3 color;

out vec3 Color;
out vec2 Texpos;

void
main()
{
	Color = color;
	Texpos = texpos;
	gl_Position = vec4(position, 0.0, 1.0);
}
