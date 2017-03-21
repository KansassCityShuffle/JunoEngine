#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform vec2 offset[2048];


void main()
{
  vec2 off = offset[2];
  gl_Position = worldToView * modelToWorld * vec4(position.x + off.x, position.y, position.z + off.y, 1.0);
  vColor = vec4(color, 1.0f);
}
