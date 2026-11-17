#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_text_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform float Time;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_color;
out vec3 frag_normal;
//out vec2 frag_text_coord;

void main()
{
    // TODO(student): Send output to fragment shader
    vec3 timePos = v_position;
    timePos.y += sin(Time) * 0.5f;

    frag_position = v_position;
    
    frag_color = v_color;
    frag_color.x += cos(Time * 3);
    frag_normal = v_normal;
  //  frag_text_coord = v_text_coord;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(timePos, 1.0);
}
