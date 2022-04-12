#include "Sample.h"

Sample::Sample(const Color& color, const Ray& ray, float pdf, float rpdf, bool specular) : color(color), outRay(ray), pdf(pdf), specular(specular) {
}