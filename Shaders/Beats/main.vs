varying vec3 normal; // the normal
varying vec3 vertex; // the vertex
varying float distancez; // the z distance away from the camera

void main(void)
{
   // pass the color

   gl_FrontColor = gl_Color;

   // calculate normal

   normal = normalize(gl_NormalMatrix * gl_Normal);

   vertex = vec3(gl_ModelViewProjectionMatrix * gl_Vertex);

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   distancez = gl_Position.z;

   gl_TexCoord[0] = gl_MultiTexCoord0;
}