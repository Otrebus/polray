//#include "AreaLight.h"
//#include "Windows.h"
//#include "EmissiveMaterial.h"
//#include "Renderer.h"
//#include "Triangle.h"
//#include "Utils.h"
//#include "Scene.h"
//#include "UniformEnvironmentLight.h"
//
//UniformEnvironmentLight::UniformEnvironmentLight()
//{
//    material = new EmissiveMaterial();
//}
//
//UniformEnvironmentLight::UniformEnvironmentLight(const Vector3d& position, const Vector3d& corner1, const Vector3d& corner2, const Color& color) : pos(position), c1(corner1), c2(corner2)
//{
//    material = new EmissiveMaterial();
//    intensity_ = color;
//#ifdef DETERMINISTIC
//    r.Seed(0);
//#else
//    r.Seed(GetTickCount() + int(this));
//#endif
//}
//
//void UniformEnvironmentLight::AddToScene(std::shared_ptr<Scene> scn)
//{
//    Scene::LightAdder::AddLight(*scn, this);
//}
//
//double UniformEnvironmentLight::GetArea() const
//{
//    return radius*radius*4*M_PI;
//}
//
//double UniformEnvironmentLight::Intersect(const Ray& ray) const
//{
//    double t;
//    Vector3d dir(ray.direction);
//    Vector3d vec = ray.origin - position;
//
//    double C = vec*vec - radius*radius;
//    double B = 2*(vec*dir);
//    double A = dir*dir;
//
//    double D = (B*B/(4*A) - C)/A;
//
//    t = -B/(2*A) - sqrt(D);
//            
//    if(D > 0) {
//        if(t < eps)
//            return -B/(2*A) + sqrt(D) > 0 ? t = -B/(2*A) + sqrt(D) : -inf;
//        return t;
//    }
//    return -inf;
//}
//
//bool UniformEnvironmentLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
//{
//    double t;
//    Vector3d dir(ray.direction);
//    Vector3d vec = ray.origin - position;
//
//    info.direction = ray.direction;
//    info.material = material;
//
//    double C = vec*vec - radius*radius;
//    double B = 2*(vec*dir);
//    double A = dir*dir;
//
//    double D = (B*B/(4*A) - C)/A;
//
//    t = -B/(2*A) - sqrt(D);
//
//    if(D >= 0)
//    {
//        if(t < eps)
//        {
//            t = -B/(2*A) + sqrt(D);
//            if(t < eps)
//                return false;
//        }
//        info.normal = (ray.origin + ray.direction*t) - position;
//        info.normal.Normalize();
//        info.position = ray.origin + ray.direction*(t - eps);
//
//        // Texture coordinates - there are probably better methods than this one
//        Vector3d v = info.position - position;
//        Vector3d w = (up^v)^up;
//        Vector3d forward = up^right;
//        v.Normalize();
//        w.Normalize();
//        forward.Normalize();
//
//        double vcoord = acosf(up*v) / 3.14159265f;
//        double ucoord;
//
//        // Clamp the coordinates to prevent NaNs, which is one of the reasons this method is inferior
//        double wright = w*right > 1 ? 1 : w*right < -1 ? -1 : w*right;
//
//        if(w*forward >= 0)
//            ucoord = acos(wright) / (2.0f*3.14159265f);
//        else
//            ucoord = 1.0f - acos(wright) / (2.0f*3.14159265f);
//        
//        info.texpos.x = ucoord;
//        info.texpos.y = vcoord;
//
//        info.geometricnormal = info.normal;
//        return true;
//    }
//    return false;
//}
//
//double UniformEnvironmentLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
//{
//
//}
//
//Color UniformEnvironmentLight::SampleRay(Ray& ray, Vector3d& n, double& areaPdf, double& anglePdf) const
//{
//}
//
//void UniformEnvironmentLight::SamplePoint(Vector3d& point, Vector3d& n) const
//{
//}
//
//Vector3d UniformEnvironmentLight::GetNormal() const
//{
//}
//
//void UniformEnvironmentLight::Save(Bytestream& stream) const
//{
//}
//
//void AreaLight::Load(Bytestream& stream)
//{
//}
//
//Color UniformEnvironmentLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln, int component) const
//{
//}
//
//Color UniformEnvironmentLight::DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo, int component) const
//{
//}
//
//Color UniformEnvironmentLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const
//{
//}
