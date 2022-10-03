#include "Sample.h"

/**
 * Constructor.
 * 
 * @param color The estimate of the sapmle.
 * @param ray The outgoing ray sampled.
 * @param pdf The value of the probability distribution function of the direction of the sample.
 * @param rpdf The value of the probability distribution function of the incoming ray of the sample.
 * @param specular Whether the material is specular.
 * @param component The component of the BRDF that we sampled.
 */
Sample::Sample(const Color& color, const Ray& ray, double pdf, double rpdf, bool specular, int component) : color(color), outRay(ray), pdf(pdf), rpdf(rpdf), specular(specular), component(component)
{
    assert(pdf >= 0);
    assert(rpdf >= 0);
    assert(color.IsValid());
}

/**
 * Constructor.
 */
Sample::Sample()
{
}