#version 330 core
layout(location = 0) in highp vec3 position;
layout(location = 1) in mediump vec2 texCoord;
out vec2 texC;

uniform mediump mat4 modelToWorld;
uniform mediump mat4 worldToView;

void main(void)
{
    gl_Position = worldToView * modelToWorld * vec4(position, 1.0);
    texC = texCoord;
}
