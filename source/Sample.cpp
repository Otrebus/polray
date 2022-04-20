#include "Sample.h"

Sample::Sample(const Color& color, const Ray& ray, float pdf, float rpdf, bool specular) : color(color), outRay(ray), pdf(pdf), rpdf(rpdf), specular(specular) {
    assert(pdf >= 0);
    assert(rpdf >= 0);
    assert(color.IsValid());
}

Sample::Sample() {
}