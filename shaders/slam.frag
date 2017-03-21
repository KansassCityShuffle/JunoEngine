#version 330
in highp vec4 vColor;
out highp vec4 fColor;

void main()
{
   fColor = vColor;
   //fColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}
