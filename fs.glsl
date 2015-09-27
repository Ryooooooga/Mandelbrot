
uniform vec2	size;
uniform float	scale;

float mandelbort(float a, float b) {
	int		n	= int(30.0 * scale);
	float	lim	= 32.0;

	float x = 0.0;
	float y = 0.0;

	for(int i=0; i<n; i++) {
		float xn = x*x - y*y + a;
		float yn = 2.0*x*y + b;
		x = xn;
		y = yn;

		if(x*x + y*y > lim)
			return float(i)/float(n);
	}

	return 1.0;
}

void main() {
	float c = mandelbort(size.x * (gl_Color.x-0.5), size.y * (gl_Color.y - 0.5));
	gl_FragColor = vec4(c, c, c, 1.0);
}
