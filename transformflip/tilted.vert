#version 330 core
in vec2 texpos;
in vec2 position;
out vec2 Texpos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 flip;
void main()
{
	vec4 foo;
	Texpos = texpos;
	gl_Position =  proj * view * flip * model * vec4(position, 0.0, 1.0);
}
