const mediump vec2 BlockSize = vec2(0.15, 0.15);
varying mediump vec2 frag_uv;
varying mediump vec3 H;

const mediump float GratingSpacing = 1800.0;
const mediump float SurfaceRoughness = 1.2;
const lowp vec4 HighlightColor = vec4(1.0, 1.0, 1.0, 1.0);
const lowp vec4 DarkColor = vec4(1.0, 1.0, 0.0, 1.0);

mediump vec3 lambda2rgb(mediump float lambda)
{
    const mediump float ultraviolet = 400.0;
    const mediump float infrared    = 700.0;

    mediump float a = (lambda - ultraviolet) / (infrared - ultraviolet);

    const mediump float C = 7.0;
    mediump vec3 b = vec3(a) - vec3(0.75, 0.5, 0.25);
    return max((1.0 - C * b * b), 0.0);
}

void main()
{
    lowp vec3 color;
    mediump vec2 position;

    position = fract(frag_uv / BlockSize);

    mediump vec3 T = normalize(vec3((-position.y + 0.5), (-position.x + 0.5), 0.0));
    mediump float u = abs(dot(T, H));
    mediump vec3 diffColor = vec3(0.0);

    const int numSpectralOrders = 3;
    for (int m = 1; m <= numSpectralOrders; ++m)
    {
        mediump float lambda = GratingSpacing * u / float(m);
        diffColor += lambda2rgb(lambda);
    }
    mediump vec3 N = vec3(0.0, 0.0, 1.0);
    mediump float w = dot(N, H);
    mediump float e = SurfaceRoughness * u / w;
    mediump vec3 hilight = exp(-e * e) * HighlightColor.rgb;
    const mediump float diffAtten = 0.8; 
    gl_FragColor = mix(vec4(diffAtten * diffColor + hilight, 1.0), DarkColor, 0.7);
}
