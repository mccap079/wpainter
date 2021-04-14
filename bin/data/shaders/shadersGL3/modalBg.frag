#version 150

out vec4 outputColor;

void main()
{   
    vec2 p = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));

	if(mod(p.y,2.0) == 0){
		if(mod(p.x,2.0) != 0) discard;
	} else {
		if(mod(p.x,2.0) == 0) discard;
	}

	outputColor = vec4(0.0,0.0,0.0,1.0);
}