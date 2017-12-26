#version 400  

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 model;

in vec3 VertexPosition;  
in vec3 VertexBarycentric;
  
out vec3 Barycentric;  
  
void main()  
{  
    Barycentric = VertexBarycentric;  
    gl_Position = projection * camera * model * vec4(VertexPosition,1.0);  
} 
