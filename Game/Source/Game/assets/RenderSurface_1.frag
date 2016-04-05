#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float time = 0;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float x, float y){
	return rand(vec2(x,y));
}

void main() {
	// original
	vec4 orig = vec4(texture(texFramebuffer, Texcoord) );
	if(orig.g >= 0.9){
		outColor = orig;
		return;
	}

	// get pixels
	vec2 r = Texcoord;
	r *= 64;
	r -= mod(r, 1);
	r /= 64;
	
	// distort uvs
	r.y += sin(time*8+r.x*10)*(1+sin(time*11+r.x*3 + rand(r.y, sin(time))))*0.02;
	r.y = abs(rand(r.x, cos(time))*0.005 + 0.5 - r.y);

	// new
	vec4 col = vec4(texture(texFramebuffer, r) );
	col = vec4(r.y, r.x*r.y, 0, 1);
	// new
	vec4 col2 = vec4(texture(texFramebuffer, r) );
	col2 = vec4(1.f-r.y, (1.f-r.x)*(1.f-r.y), 0, 1);

	// combined
    outColor = orig + (col * col2 - orig.r) * r.y * (distance(r, vec2(0.5))+0.25)*2;
}
