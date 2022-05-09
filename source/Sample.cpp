#include "Sample.h"

Sample::Sample(const Color& color, const Ray& ray, double pdf, double rpdf, bool specular, int component) : color(color), outRay(ray), pdf(pdf), rpdf(rpdf), specular(specular), component(component) {
    //assert(pdf >= 0);
    //assert(rpdf >= 0);
    //assert(color.IsValid());
}

Sample::Sample() {
}