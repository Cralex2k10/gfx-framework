#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// TODO(student): Output values to fragment shader
layout(location = 0) out vec3 frag_color;

void main()
{
    // TODO(student): Send output to fragment shader
    v_position = vect4(1, 0, 0, 0);
    frag_color = vertex+col

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
