#version 130

/// TODO: test this shader

precision highp float;

uniform float animVal;

void main()
{
    vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));

	if(mod(p.y,10.0) == 0.0){
		if(mod(p.x,10.0) == 0.0) discard;
	}

	if(mod(p.y,10.0) > ceil(animVal)) discard;
	if(mod(p.x,10.0) > ceil(animVal)) discard;

	gl_FragColor = vec4(0.0,0.0,0.0,1.0);
}
