

uniform sampler2D IN_TEX;
uniform sampler3D IN_LUT;

const vec3 SIZE = vec3(8.0, 8.0, 8.0);
const vec3 SCALE = (SIZE - 1.0) / SIZE;
const vec3 OFFSET = 1.0 / (2.0 * SIZE);

void main ()
{
	vec3 index = texture2D(IN_TEX, gl_TexCoord[0].xy).xyz;
	gl_FragColor = texture3D(IN_LUT, index*SCALE + OFFSET);
}
