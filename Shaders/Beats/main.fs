varying vec3 normal; // the normal
varying vec3 vertex; // the vertex

uniform sampler2D texture1; // the texture of the image

varying float distancez; // the z distance away from the camera

float fardistance = 9.0f; // the furthest visible distnce

vec4 fadetocolor = vec4(.1, .1, .1, 1);

void main (void)
{
   //float mixshadow = distancez / fardistance; // influence of shadow

   vec3 light = normalize(vec3(0, 1, 3) - vertex);

   vec4 diff = gl_Color * max(dot(normal, light), 0.0);

   diff = clamp(diff, 0.0, 1.0);

   gl_FragColor = vec4(diff.xyz, gl_Color.w);
}