#version 150

/// TODO: test this shader

out vec4 outputColor;

uniform float animVal;
uniform float translateVal;

void main()
{   
    vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));

	if(mod(p.y,10.0) == ceil(translateVal)){
		if(mod(p.x,10.0) == ceil(translateVal)) discard;
	}

	if(mod(p.y,10.0) > ceil(animVal)) discard;
	if(mod(p.x,10.0) > ceil(animVal)) discard;

	outputColor = vec4(0.0,0.0,0.0,1.0);
}