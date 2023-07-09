/*#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}*/

#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0,1.0,0.5,0.0);
}