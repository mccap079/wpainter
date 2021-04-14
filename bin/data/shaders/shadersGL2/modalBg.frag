#version 120
/// This one is compiling on debug machine

void main()
{
	vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));

	if(mod(p.y,2.0) == 0){
		if(mod(p.x,2.0) != 0) discard;
	} else {
		if(mod(p.x,2.0) == 0) discard;
	}

	gl_FragColor = vec4(0.0,0.0,0.0,1.0);
}