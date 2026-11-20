#version 330

// Output
layout(location = 0) out vec4 out_color;
//layout(location = 1) out vec4 out_normal;
//layout(location = 2) out vec4 out_position;

in vec3 frag_position;
in vec3 frag_color;
in vec3 frag_normal;
in vec2 frag_text_coord;

void main()
{
    // TODO(student): Write pixel out color

    //out_position = vec4(frag_position, 1);
    out_color = vec4(frag_color, 1);   // change back to color for Time variation
    //out_normal = vec4(frag_normal, 1);
}