#version 400  

in vec3 Barycentric;  

uniform vec3 setcolor;
uniform int coutedge;

out vec4 FragColor;

#extension GL_OES_standard_derivatives : enable
float edgeFactor(){
    vec3 d = fwidth(Barycentric);
    float coefficient = 0.8;
    if(coutedge==4)
        coefficient=0.3;
    else
        coefficient=0.8;
    vec3 a3 = smoothstep(vec3(0.0), d * coefficient, Barycentric);
    return min(min(a3.x, a3.y), a3.z);
}

void main()
{
    if(coutedge==2)
        FragColor = vec4(setcolor,1.0);
    else if(coutedge==3 || coutedge==4)
    {
        FragColor.rgb = mix(vec3(0.0), setcolor, edgeFactor());
    }
    else
        FragColor = vec4(0.5,0.5,0.5,1.0);
}
