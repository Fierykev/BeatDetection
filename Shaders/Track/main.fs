uniform sampler2D texture0; // the texture of the image

uniform sampler2D texture1; // the texture of the image

uniform float lane[4]; // the lanes that should be lit up

uniform float error[4]; // the user has made an error

varying float distancez; // the distance away from the camera

varying vec2 pixelpos; // the position of the pixel on the screen

float fardistance = 9.0f; // the furthest visible distance squared

float beatbarheight = .2; // the length of the beat bar

vec4 fadetocolor = vec4(.1, .1, .1, 1);

vec4 errorcolor = vec4(1, 0, 0, 1);

// color lights

vec4 lightupcolor[4] = vec4[](vec4(1, 0, 1, 1),
                       vec4(0, 1, 1, 1),
                       vec4(0, 0, 1, 1),
                       vec4(1, 1, 0, 1));

void main (void)
{
   float mixshadow = distancez / fardistance; // influence of shadow

   vec4 color;

   if (pixelpos.y < 0)
      color = vec4(0,0,0,1);
   else 
   {
      if (pixelpos.y <= beatbarheight)
         color = texture2D(texture1, vec2(gl_TexCoord[0].s, pixelpos.y / beatbarheight));
      else
         color = texture2D(texture0, gl_TexCoord[0].st);

      for (int i = 0; i < 4; i++)
            if (error[i] == 1.0)
            {
               if (gl_TexCoord[0].s <= 1.0 / 4.0 * (i + 1.0) && 1.0 / 4.0 * i <= gl_TexCoord[0].s)
                  color = color * .6 + errorcolor * .4;
            }
            else if (lane[i] == 1.0)
            {
               if (gl_TexCoord[0].s <= 1.0 / 4.0 * (i + 1.0) && 1.0 / 4.0 * i <= gl_TexCoord[0].s)
                        color = color * .6 + lightupcolor[i] * .4;
            }
   }

   gl_FragColor = color * (1.0 - mixshadow) + fadetocolor * mixshadow;
}