varying float distancez; // the distance away from the camera

varying vec2 pixelpos; // the position of the pixel on the screen

void main(void)
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   pixelpos = (1.0 + gl_Position.xy / gl_Position.w) / 2.0;

   distancez = gl_Position.z;

   gl_TexCoord[0] = gl_MultiTexCoord0;
}