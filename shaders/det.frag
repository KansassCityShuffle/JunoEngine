#version 330
uniform sampler2D texture;
in vec2 texC;
out vec4 fColor;

void main(void)
{
    fColor = texture2D(texture, texC.st);
}
