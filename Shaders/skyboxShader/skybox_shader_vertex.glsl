#version 410 core
layout (location = 0) in vec3 pos_in;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 PV;

void main()
{
    tex_coords = pos_in;
    vec4 pos = PV * vec4(pos_in, 1.0);
    gl_Position = pos.xyww;
}
