#include "CsgIntersection.h"
#include "CsgSphere.h"
#include "CsgCylinder.h"
#include "CsgCuboid.h"
#include "Randomizer.h"
#include "LightTracer.h"
#include "LightPortal.h"
#include "Color.h"
#include "draw.h"
#include "main.h"
#include "Vector3d.h"
#include "Ray.h"
#include "logger.h"
#include <cmath>
#include "timer.h"
#include "sphere.h"
#include <vector>
#include "triangle.h"
#include "kdtree.h"
#include "trianglemesh.h"
#include "Scene.h"
#include "MeanEstimator.h"
#include "MonEstimator.h"
#include "RayTracer.h"
#include "PathTracer.h"
#include "AreaLight.h"
#include "PhongMaterial.h"
#include "MirrorMaterial.h"
#include "DielectricMaterial.h"
#include "BDPT.h"
#include "Matrix3d.h"
#include <fstream>
#include <sstream>
#include "ThinLensCamera.h"
#include "PinholeCamera.h"
#include "AshikhminShirley.h"
#include "LambertianMaterial.h"
#include "SphereLight.h"
#include "MeshLight.h"
#include "BrutePartitioning.h"
#include "UniformEnvironmentLight.h"

//#define INTERIOR
//#define CUBE
//#define TEAPOTWITHOUTNORMALS
//#define INTERIORSKY
//#define INTERIORINLIGHT
//#define INTERIORFOG
//#define BOX
//#define MESHLIGHTBOX
//#define ROOM
//#define EMPTYBOX
#define KITCHEN2
//#define WINDOWBOX
//#define WINDOWBOX2
//#define BALLSBOX
//#define CONFERENCE
//#define BALLBOX
//#define LEGOCAR
//#define KITCHEN
//#define DODGE
//#define TEAPOT
//#define OCLBOX
//#define SHINYBALL
//#define CSGTEST
//#define SAMPLES_PER_PIXEL 1

void MakeScene(std::shared_ptr<Renderer>& r, std::shared_ptr<Estimator>& e)
{
#ifdef KITCHEN2
    auto s = std::shared_ptr<Scene> (new Scene("Morning Apartment2.obj"));

    Vector3d camPos = Vector3d(0.6, 2.0, 2.5);
    Vector3d target = Vector3d(-1.4, 1.35, -2.1);

    //Vector3d camPos = Vector3d(2.1, 3.1, -0.4);
    //Vector3d target = Vector3d(-1.8, 0.6, -2.9);

    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 70, (Vector3d(0, 1.9, -3.2)-camPos).Length(), 0.02));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 70));

    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Randomizer ballsR(47);

    Randomizer ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));


    //auto counterlight = new AreaLight(
    //v -0.745242 2.069140 -3.669199
    //v -1.447420 2.069140 -3.669199
    //v -0.745242 2.069140 -4.189088
    //v -1.447420 2.069140 -4.189088


    // auto boxLight = new SphereLight(Vector3d(1.3, 3.6, 0), 0.05, Color(1000, 1000, 1000));



    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(-2.7, 3.1, -3.5), Vector3d(0, -1.7, 0), Vector3d(0, 0, 2.7));

    LightPortal* portalLight2 = new LightPortal();
    portalLight2->AddPortal(Vector3d(-2.7, 3.1, -3.5), Vector3d(0, -1.7, 0), Vector3d(0, 0, 2.7));

    auto sunLight = new SphereLight(Vector3d(-18.4, 8.4, 4)*100, 10, 8000*Color(233, 237, 147));
    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100000, 3*Color(0.9, 1.2, 1.5));

    portalLight->SetLight(sunLight);
    portalLight2->SetLight(skyLight);

    //////
    ///*s->AddLight(sunLight);
    //s->AddLight(skyLight);*/
    s->AddLight(portalLight);
    s->AddLight(portalLight2);

    //s->AddLight(boxLight);

    auto tr = Vector3d(4.0, 4.1, 3.2);
    auto tl = Vector3d(-2.5, 4.1, 3.2);
    auto bl = Vector3d(-2.5, 0.0, 3.2);
    auto br = Vector3d(4.0, 0.0, 3.2);

    auto mat = new LambertianMaterial();
    mat->Kd = Color(0.8, 0.8, 0.8);

    auto triangle1 = new Triangle(tl, tr, br);
    auto triangle2 = new Triangle(tl, bl, br);
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);

    tl = Vector3d(-2.6, 3.2, -0.3);
    tr = Vector3d(-2.6, 3.2, 2.9);
    bl = Vector3d(-2.6, -0.2, -0.3);
    br = Vector3d(-2.6, -0.2, 2.9);
    triangle1 = new Triangle(tl, tr, br);
    triangle2 = new Triangle(tl, bl, br);
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));
#endif
#ifdef INTERIOR
    auto s = std::shared_ptr<Scene> (new Scene("interior-open.obj"));

    Vector3d camPos = Vector3d(-118, 254.8, 544);
    Vector3d target = Vector3d(157, 159, -209);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).Length(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    portalLight->SetLight(boxLight);
    s->AddLight(portalLight);
    ////
    /*auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(500000, 500000, 500000));
    boxLight->AddToScene(s);

    s->AddLight(boxLight);*/

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<PathTracer>(new PathTracer(s));
#endif
#ifdef CUBE
    auto s = std::shared_ptr<Scene> (new Scene("cube.obj"));

    Vector3d camPos = Vector3d(2, 2, 2);
    Vector3d target = Vector3d(0, 0, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).Length(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    portalLight->SetLight(boxLight);
    s->AddLight(portalLight);
    ////
    /*auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(500000, 500000, 500000));
    boxLight->AddToScene(s);

    s->AddLight(boxLight);*/

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<RayTracer>(new RayTracer(s));
#endif
#ifdef TEAPOTWITHOUTNORMALS
    auto s = std::shared_ptr<Scene> (new Scene("teapotwithoutnormals.obj"));

    Vector3d camPos = Vector3d(30, -30, 30);
    Vector3d target = Vector3d(0, 0, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).Length(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    portalLight->SetLight(boxLight);
    s->AddLight(portalLight);
    ////
    /*auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(500000, 500000, 500000));
    boxLight->AddToScene(s);

    s->AddLight(boxLight);*/

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<RayTracer>(new RayTracer(s));
#endif
#ifdef INTERIORSKY
    auto s = std::shared_ptr<Scene> (new Scene("interior-open.obj"));

    Vector3d camPos = Vector3d(-118, 254.8, 500);
    Vector3d target = Vector3d(157, 159, -209);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).Length(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Randomizer ballsR(47);
    Randomizer test(1);

    Randomizer ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    Vector3d ul(-207, 385, 508);
    Vector3d ur(349, 391, 508);
    Vector3d bl(-217, -8.5, 508);
    Vector3d br(347, -7.8, 508);

    auto mat = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle1 = new Triangle(ul, ur, br);
    auto triangle2 = new Triangle(ul, bl, br);
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);


    Vector3d ul2(-210, 787, -625);
    Vector3d ur2(-210, 770, -166);
    Vector3d bl2(-210, 476, -602);
    Vector3d br2(-210, 454, -150);

    auto mat2 = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle12 = new Triangle(ul2, ur2, br2);
    auto triangle22 = new Triangle(ul2, bl2, br2);
    triangle12->SetMaterial(mat2);
    triangle22->SetMaterial(mat2);
    s->AddModel(triangle12);
    s->AddModel(triangle22);

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    portalLight->SetLight(boxLight);
    //s->AddLight(boxLight);
    s->AddLight(portalLight);
    ////

    LightPortal* portalLight2 = new LightPortal();
    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100000, 15*Color(0.9, 1.2, 1.5));
    portalLight2->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    portalLight2->SetLight(skyLight);
    s->AddLight(portalLight2);

    /*auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(500000, 500000, 500000));
    boxLight->AddToScene(s);

    s->AddLight(boxLight);*/

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));
#endif
#ifdef INTERIORINLIGHT
    auto s = std::shared_ptr<Scene> (new Scene("interior-open3-lights.obj"));

    Vector3d camPos = Vector3d(-118, 254.8, 500);
    Vector3d target = Vector3d(157, 159, -209);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).GetLength(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));

    //Vector3d camPos = Vector3d(116, 225, -248);
    //Vector3d target = Vector3d(-38, 186, -947);
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    Vector3d ul(-207, 385, 508);
    Vector3d ur(349, 391, 508);
    Vector3d bl(-217, -8.5, 508);
    Vector3d br(347, -7.8, 508);

    auto mat = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle1 = new Triangle(ul, ur, br);
    auto triangle2 = new Triangle(ul, bl, br);
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);


    Vector3d ul2(-210, 787, -625);
    Vector3d ur2(-210, 770, -166);
    Vector3d bl2(-210, 476, -602);
    Vector3d br2(-210, 454, -150);

    auto mat2 = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle12 = new Triangle(ul2, ur2, br2);
    auto triangle22 = new Triangle(ul2, bl2, br2);
    triangle12->SetMaterial(mat2);
    triangle22->SetMaterial(mat2);
    s->AddModel(triangle12);
    s->AddModel(triangle22);

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    //auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    //portalLight->SetLight(boxLight);
    //s->AddLight(boxLight);
    //s->AddLight(portalLight);
    ////

    LightPortal* portalLight2 = new LightPortal();
    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100000, 15*Color(0.9, 1.2, 1.5));
    portalLight2->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    portalLight2->SetLight(skyLight);
    //s->AddLight(portalLight2);

    //auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(150000, 150000, 150000));
    auto boxLight = new AreaLight(Vector3d(-43.9, 232, 43.4), Vector3d(0, 1.0, 0), Vector3d(0, 0, 1.0), Color(250000, 250000, 250000));
    //s->AddLight(boxLight);

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));
#endif
#ifdef INTERIORFOG
    auto s = std::shared_ptr<Scene> (new Scene("interior-open3.obj"));

    Vector3d camPos = Vector3d(-118, 254.8, 500);
    Vector3d target = Vector3d(157, 159, -209);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(120, 161, -139)-camPos).GetLength(), 10.15));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));


    // tmp
    //Vector3d camPos = Vector3d(-43.9, 293.8, 43);
    //Vector3d target = Vector3d(8.6, 230, -301);
    ///*Vector3d camPos = Vector3d(-150, 184, -760);
    //Vector3d target = Vector3d(-119, 139, -914);*/
    //Vector3d camdir = target-camPos;
    //camdir.Normalize();
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 40));

    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //auto portalLight = new AreaLight(Vector3d(-79, 387, 263), Vector3d(0, 15.0, 0.0), Vector3d(0.0, 0.0, 15.0), Color(7500, 7500, 7500));
    //auto portalLight = new SphereLight(Vector3d(301, 370, -154), 0.11, Color(5000000, 5000000, 5000000));
    //auto portalLight = new SphereLight(Vector3d(3854, 1750, -1770), 30.11, Color(5000000, 5000000, 5000000));

    //auto boxLight = new SphereLight(Vector3d(3854, 1750, -1770)*100, 550.11, Color(50000000, 50000000, 50000000));

    Vector3d ul(-207, 385, 508);
    Vector3d ur(349, 391, 508);
    Vector3d bl(-217, -8.5, 508);
    Vector3d br(347, -7.8, 508);

    auto mat = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle1 = new Triangle(ul, ur, br);
    auto triangle2 = new Triangle(ul, bl, br);
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);


    Vector3d ul2(-210, 787, -625);
    Vector3d ur2(-210, 770, -166);
    Vector3d bl2(-210, 476, -602);
    Vector3d br2(-210, 454, -150);

    auto mat2 = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle12 = new Triangle(ul2, ur2, br2);
    auto triangle22 = new Triangle(ul2, bl2, br2);
    triangle12->SetMaterial(mat2);
    triangle22->SetMaterial(mat2);
    s->AddModel(triangle12);
    s->AddModel(triangle22);

    //////
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    auto boxLight = new AreaLight(Vector3d(3854, 1750, -1770)*100, Vector3d(0.0, 0.0, 5050), Vector3d(0.0, 5050, 0.0), Color(130000000/25, 130000000/25, 130000000/25));
    portalLight->SetLight(boxLight);
    //s->AddLight(boxLight);
    //s->AddLight(portalLight);
    ////

    auto counterLight = new AreaLight(Vector3d(14.2, 230, -971), Vector3d(70, 0, 0), Vector3d(0, 0, 30), Color(100, 90, 65)/4);
    s->AddLight(counterLight);

    LightPortal* portalLight2 = new LightPortal();
    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100000, 15*Color(1.3, 1.4, 1.5));
    portalLight2->AddPortal(Vector3d(500, 800, -1050), Vector3d(0, -800, 0), Vector3d(0, 0, 1600));
    portalLight2->SetLight(skyLight);
    s->AddLight(portalLight2);

    /*auto boxLight = new SphereLight(Vector3d(-43.9, 292, 43.4), 1.11, Color(500000, 500000, 500000));
    boxLight->AddToScene(s);

    s->AddLight(boxLight);*/

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));
#endif
#ifdef ROOM
    auto s = std::shared_ptr<Scene> (new Scene("Room.obj"));

    Vector3d camPos = Vector3d(-39.5, -10, 324.6);
    Vector3d target = Vector3d(-60.1, -30.8, 172.1);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(-0.6, 0.5, 0.4)-camPos).GetLength(), 0.15));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));

    Randomizer ballsR(47);
    Randomizer test(1);

    Randomizer ballsC(0);

    //SphereLight* boxLight = new SphereLight(Vector3d(2, 1.0, 0), 0.11, Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(1500, 1500, 1500), s);
    //boxLight->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));
    //boxLight->AddPortal(Vector3d(1, 1.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));

    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100000, 3.5*Color(0.9, 1.2, 1.5));

    auto sunLight = new SphereLight(Vector3d(-40, 16, 18)*2000, 500, 2400*Color(255, 247, 223));
    //auto sunLight = new AreaLight(Vector3d(-40, 16, 18)*2000+Vector3d(0, 0, -250)+Vector3d(0, -250, 0), Vector3d(0.0, 1, 0)*500, Vector3d(0, 0, 1)*500, 4400*Color(255, 247, 223));
    
    auto inLight = new SphereLight(Vector3d(-80, -25, 90), 10, Color(2500, 2500, 2500));
    //auto inLight = new AreaLight(Vector3d(-80, 25, 90), Vector3d(0.0, 0.0, 4), Vector3d(0, 4, 0), Color(4000, 4000, 4000));


    auto mat = new LambertianMaterial();
    mat->Kd = Color(0.2, 0.2, 0.2);

    auto triangle1 = new Triangle(Vector3d(-180, -115, 345), Vector3d(260, 115, 345), Vector3d(260, -115, 345));
    auto triangle2 = new Triangle(Vector3d(-180, 115, 345), Vector3d(260, 115, 345), Vector3d(-180, -115, 345));
    triangle1->SetMaterial(mat);
    triangle2->SetMaterial(mat);
    s->AddModel(triangle1);
    s->AddModel(triangle2);

    //AreaLight* sunLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 10), Vector3d(10, 0.0, 0.0), Color(4000, 4000, 4000));
    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(-270, 140, -354), Vector3d(0, -280, 0), Vector3d(0, 0, 870));
    
    /*portalLight->AddPortal(Vector3d(-270, 114, -354), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight->AddPortal(Vector3d(-270, 114, -154), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight->AddPortal(Vector3d(-270, 114, 54), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight->AddPortal(Vector3d(-270, 114, 254), Vector3d(0, -250, 0), Vector3d(0, 0, 200));*/
    
    //portalLight->AddPortal(Vector3d(-270, 100, -120), Vector3d(0, -140, 0), Vector3d(0, 0, 150));
    //portalLight->AddPortal(Vector3d(-270, 100, 60), Vector3d(0, -140, 0), Vector3d(0, 0, 150));

    //portalLight->AddPortal(Vector3d(-271, 87, -106), Vector3d(0, -130, 0), Vector3d(0, 0, 130));
    //portalLight->AddPortal(Vector3d(-270, 87, 77), Vector3d(0, -130, 0), Vector3d(0, 0, 130));
    
    portalLight->SetLight(sunLight);

    LightPortal* portalLight2 = new LightPortal();

    /*portalLight2->AddPortal(Vector3d(-270, 114, -354), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight2->AddPortal(Vector3d(-270, 114, -154), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight2->AddPortal(Vector3d(-270, 114, 54), Vector3d(0, -250, 0), Vector3d(0, 0, 200));
    portalLight2->AddPortal(Vector3d(-270, 114, 254), Vector3d(0, -250, 0), Vector3d(0, 0, 200));*/


    portalLight2->AddPortal(Vector3d(-270, 140, -354), Vector3d(0, -280, 0), Vector3d(0, 0, 870));
    
    //portalLight2->AddPortal(Vector3d(-270, 100, -120), Vector3d(0, -140, 0), Vector3d(0, 0, 170));
    //portalLight2->AddPortal(Vector3d(-270, 100, 60), Vector3d(0, -140, 0), Vector3d(0, 0, 170));
    
    //portalLight2->AddPortal(Vector3d(-271, 87, -106), Vector3d(0, 0, 130), Vector3d(0, -130, 0));
    //portalLight2->AddPortal(Vector3d(-271, 87, 77), Vector3d(0, 0, 130), Vector3d(0, -130, 0));
    
    portalLight2->SetLight(skyLight);

    s->AddLight(portalLight);
    s->AddLight(portalLight2);
    //s->AddLight(sunLight);
    //s->AddLight(skyLight);

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<LightTracer>(new LightTracer(s));
#endif

#ifdef WINDOWBOX
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-Windows.obj"));

    Vector3d camPos = Vector3d(0, 1.4, 3);
    Vector3d target = Vector3d(0, 1, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(-0.6, 0.5, 0.4)-camPos).GetLength(), 0.15));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));

    Randomizer ballsR(47);
    Randomizer test(1);

    Randomizer ballsC(0);

    //SphereLight* boxLight = new SphereLight(Vector3d(2, 1.0, 0), 0.11, Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(1500, 1500, 1500), s);
    //boxLight->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));
    //boxLight->AddPortal(Vector3d(1, 1.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));

    /*auto portalLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(3500, 3500, 3500));
    portalLight->AddToScene(s);*/

    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 10000, 2.5*Color(0.9, 1.2, 1.5));
    auto sunLight = new SphereLight(Vector3d(40, 16, 18)*20, 100, Color(550, 500, 480));

    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight->AddPortal(Vector3d(1, 0.25, -0.75), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight->SetLight(sunLight);

    LightPortal* portalLight2 = new LightPortal();
    portalLight2->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight2->AddPortal(Vector3d(1, 0.25, -0.75), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight2->SetLight(skyLight);

    //auto boxLight = new SphereLight(Vector3d(4, 1.0, 0), 0.4, Color(2000, 2000, 2000));
    //auto boxLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(3500, 3500, 3500));
    //portalLight->SetLight(boxLight);

    s->AddLight(portalLight);
    s->AddLight(portalLight2);

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    auto a = new LambertianMaterial();
    a->Kd = Color(0.5, 0.5, 0.5);

    Sphere* sphere = new Sphere(Vector3d(-0.6, 0.5, 0.2), 0.31);
    sphere->SetMaterial(a);
    s->AddModel(sphere);

    /*auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.0, 0.5), 0.51);
    sphere2->SetMaterial(g);
    s->AddModel(sphere2);*/

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));

#endif

#ifdef WINDOWBOX2
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-Windows2.obj"));

    Vector3d camPos = Vector3d(-0.9, 1.8, 0.9);
    Vector3d target = Vector3d(0.5, 0.5, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(-0.6, 0.5, 0.4)-camPos).GetLength(), 0.15));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 90));

    //SphereLight* boxLight = new SphereLight(Vector3d(2, 1.0, 0), 0.11, Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(1500, 1500, 1500), s);
    //boxLight->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));
    //boxLight->AddPortal(Vector3d(1, 1.25, 0.25), Vector3d(0, 0.5, 0), Vector3d(0, 0, 1));

    /*auto portalLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(3500, 3500, 3500));
    portalLight->AddToScene(s);*/

    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 10000, 2.5*Color(0.9, 1.2, 1.5));
    //auto sunLight = new SphereLight(Vector3d(40, 16, 18)*20, 100, Color(550, 500, 480));
    auto sunLight = new AreaLight(Vector3d(40, 16, 14), Vector3d(0.0, 0.0, 4.4), Vector3d(0.0, 4.4, 0.0), Color(3500, 3500, 3500));

    auto fakeSkyLight = new AreaLight(Vector3d(4, 4, -4), Vector3d(0, -8, 0), Vector3d(0, 0, 8), Color(10, 10, 10));

    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight->AddPortal(Vector3d(1, 0.25, -0.75), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight->SetLight(sunLight);

    LightPortal* portalLight2 = new LightPortal();
    portalLight2->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight2->AddPortal(Vector3d(1, 0.25, -0.75), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight2->SetLight(skyLight);

    LightPortal* portalLight3 = new LightPortal();
    portalLight3->AddPortal(Vector3d(1, 0.25, 0.25), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight3->AddPortal(Vector3d(1, 0.25, -0.75), Vector3d(0, 1.5, 0), Vector3d(0, 0, 0.5));
    portalLight3->SetLight(fakeSkyLight);

    //auto boxLight = new SphereLight(Vector3d(4, 1.0, 0), 0.4, Color(2000, 2000, 2000));
    //auto boxLight = new AreaLight(Vector3d(4, 1.0, 0), Vector3d(0.0, 0.0, 0.4), Vector3d(0.0, 0.4, 0.0), Color(3500, 3500, 3500));
    //portalLight->SetLight(boxLight);

    s->AddLight(portalLight);
    s->AddLight(portalLight2);

    //s->AddLight(fakeSkyLight);
    //s->AddLight(skyLight);
    //s->AddLight(skyLight);
    //s->AddLight(sunLight);

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/

    //s->SetPartitioning(new BrutePartitioning());

    auto a = new LambertianMaterial();
    a->Kd = Color(0.5, 0.5, 0.5);

    Sphere* sphere = new Sphere(Vector3d(-0.6, 0.5, 0.2), 0.31);
    sphere->SetMaterial(a);
    s->AddModel(sphere);

    /*auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.0, 0.5), 0.51);
    sphere2->SetMaterial(g);
    s->AddModel(sphere2);*/

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<BDPT>(new BDPT(s));

#endif

#ifdef EMPTYBOX
/*
    Matrix3d rotatebunny (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny (1, 0, 0, 0,
                        0, 1, 0, -0.0f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny (0.53f, 0, 0, 0,
                    0, 0.53f, 0, 0,
                    0, 0, 0.53f, 0,
                    0, 0, 0, 1);*/
    /*
        Matrix3d rotatebunny2 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny2 (1, 0, 0, 0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny2 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);
        Matrix3d rotatebunny3 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny3 (1, 0, 0, -0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny3 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);*/

    //MeshLight* bunny = new MeshLight(Color(10, 10, 10), "bunny.obj");
    //bunny->Transform(translatebunny*(scalebunny*rotatebunny));
    /*
        TriangleMesh* bunny2 = new TriangleMesh("bunny.obj", 0);
        bunny2->Transform(translatebunny2*(scalebunny2*rotatebunny2));
        PhongMaterial* bunmat = new PhongMaterial();
        bunmat->alpha = 30;
        bunmat->Kd = Color(0, 0.7, 0.1);
        bunmat->Ks = Color(0.05, 0.05, 0.05);
            TriangleMesh* bunny3 = new TriangleMesh("bunny.obj", bunmat);
            bunny3->Transform(translatebunny3*(scalebunny3*rotatebunny3));*/



    /*
    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.6f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scale (0.43f, 0, 0, 0,
                    0, 0.43f, 0, 0,
                    0, 0, 0.43f, 0,
                    0, 0, 0, 1
    TriangleMesh teapot("teapotwithnormals.obj", 0);
    teapot.Transform(translate*scale);*/
    //Renderer* boxrenderer = new RayTracer();
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-Empty-RG.obj"));

    Vector3d camPos = Vector3d(0, 1.4, 3);
    Vector3d target = Vector3d(0, 1, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, (Vector3d(-0.6, 0.5, 0.4)-camPos).GetLength(), 0.15));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));

    Randomizer ballsR(47);
    Randomizer test(1);

    Randomizer ballsC(0);

    
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.2, 1.98, -0.2), Vector3d(0.4, 0.0, 0.0), Vector3d(0.0, 0, 0.4), Color(500, 500, 500), s);
    auto triangle = new Triangle(Vector3d(1, 1.2, 1.1), Vector3d(-0.8, 1.2, 1.1), Vector3d(1, 1.2, -1.1));
    auto triangle2 = new Triangle(Vector3d(1, 1.2, -1.1), Vector3d(-0.8, 1.2, -1.1), Vector3d(-0.8, 1.2, 1.1));
    auto lam = new LambertianMaterial();
    lam->Kd = Color(0.2, 0.2, 0.2);
    triangle->SetMaterial(lam);
    triangle2->SetMaterial(lam);
    s->AddModel(triangle);
    s->AddModel(triangle2);


    //auto a = new PhongMaterial();
    //a->Ks = Color(0.9, 0.9, 0.9);
    //a->Kd = Color(0.0, 0.0, 0.0);
    //a->alpha = 20;

    //auto a = new PhongMaterial();
    //a->Ks = Color(0.5, 0.5, 0.5);
    //a->Kd = Color(0.5, 0.5, 0.5);
    //a->alpha = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0, 0, 0);
    //a->Rs = Color(0.85, 0.85, 0.85);
    //a->n = 20;

    //auto a = new AshikhminShirley();
    //a->Rd = Color(0.85, 0.85, 0.85);
    //a->Rs = Color(0, 0, 0);
    /*a->Rd = Color(0, 0, 0);
    a->Rs = Color(0.85, 0.85, 0.85);
    a->n = 1;*/

    /*auto a = new PhongMaterial();
    a->Ks = Color(0.5, 0.5, 0.5);
    a->Kd = Color(0, 0, 0);
    a->alpha = 0;*/
    s->SetPartitioning(new BrutePartitioning());
    auto a = new LambertianMaterial();
    a->Kd = Color(0.5, 0.5, 0.5);

    Sphere* sphere = new Sphere(Vector3d(-0.6, 0.5, 0.4), 0.31);
    sphere->SetMaterial(a);
    s->AddModel(sphere);

    auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.0, 0.3), 0.11);
    sphere2->SetMaterial(g);
    //sphere2->AddToScene(*s);

    auto skyLight = new UniformEnvironmentLight(Vector3d(0, 0, 0), 100, 3*Color(0.9, 1.2, 1.5));
    auto sunLight = new SphereLight(Vector3d(1, 1, 1).Normalized()*8, 1, Color(500, 500, 500));
    //AreaLight* sunLight = new AreaLight(Vector3d(1, 1, 1).Normalized()*8, Vector3d(0, 2, 0.0), Vector3d(2, 0, 0), Color(500, 500, 500));

    LightPortal* portalLight = new LightPortal();
    portalLight->AddPortal(Vector3d(-1.5, -0.5, 1.0), Vector3d(0, 3, 0), Vector3d(3, 0, 0));
    portalLight->SetLight(sunLight);

    LightPortal* portalLight2 = new LightPortal();
    portalLight2->AddPortal(Vector3d(-1.5, -0.5, 1.0), Vector3d(0, 3, 0), Vector3d(3, 0, 0));
    portalLight2->SetLight(skyLight);

    //AreaLight* boxLight = new AreaLight(Vector3d(0, 1.4, 40), Vector3d(0, 250, 0.0), Vector3d(250, 0, 0), Color(50.5, 50.5, 50.5));

    //LightPortal* portalLight = new LightPortal();
    //portalLight->AddPortal(Vector3d(-4, -4, 1.1), Vector3d(0, 8, 0), Vector3d(8, 0, 0));

    //AreaLight* boxLight = new AreaLight(Vector3d(-125, 1.4, 40+125), Vector3d(0, 250/std::sqrt(2), 250 / std::sqrt(2)), Vector3d(250, 0, 0), Color(50.5, 50.5, 50.5));
    //portalLight->SetLight(boxLight);


    //SphereLight* boxLight = new SphereLight(Vector3d(-0.45, 1.0, 0.4), 0.11, Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.7, 1.199, 0.4), Vector3d(0.25, 0.0, 0.0), Vector3d(0.0, 0, 0.25), Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.7, 1.199, 0.4), Vector3d(1.5, 0.0, 0.0), Vector3d(0.0, 0, 0.1), Color(200, 200, 200));
    s->AddLight(skyLight);
    s->AddLight(sunLight);

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<LightTracer>(new LightTracer(s));

#endif

#ifdef SHINYBALL
    auto s = std::shared_ptr<Scene> (new Scene("cube.obj"));
    Vector3d camPos = Vector3d(0.9f, 0.9f, 2.5f);
    Vector3d target = Vector3d(0, 0.3f, 1.5f);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -0.75), Vector3d(0, 0, 1.0018), XRES, YRES, 75, 2.4, 0.05));
    AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);

    Sphere* sp = new Sphere(Vector3d(-0.6, 0, 2), 0.3);
    //PhongMaterial* pm = new PhongMaterial();
    AshikhminShirley* pm = new AshikhminShirley();
    pm->n = 1000000;
    //pm->alpha = 0;
    //pm->Kd = Color(0.3, 0, 0);
    pm->Rd = Color(0, 0, 0);
    pm->Rs = Color(1, 1, 1);
    //pm->Ks = Color(0.05, 0.05, 0.05);
    //pm->Ks = Color(0.0, 0.0, 0.0);
    cubearealight->AddToScene(s);
    sp->SetMaterial(pm);
    sp->AddToScene(*s);

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    //r = std::shared_ptr<LightTracer>(new LightTracer(s));
    //r = std::shared_ptr<PathTracer>(new PathTracer(s));
    r = std::shared_ptr<BDPT>(new BDPT(s));
    //r = new BDPT();
    r->SetSPP(1);
#endif
#ifdef BALLSBOX

    /*
    Matrix3d rotatebunny (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny (1, 0, 0, 0,
                        0, 1, 0, -0.0f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny (0.53f, 0, 0, 0,
                    0, 0.53f, 0, 0,
                    0, 0, 0.53f, 0,
                    0, 0, 0, 1);*/
    /*
        Matrix3d rotatebunny2 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny2 (1, 0, 0, 0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny2 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);
        Matrix3d rotatebunny3 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny3 (1, 0, 0, -0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny3 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);*/

    //MeshLight* bunny = new MeshLight(Color(10, 10, 10), "bunny.obj");
    //bunny->Transform(translatebunny*(scalebunny*rotatebunny));
    /*
        TriangleMesh* bunny2 = new TriangleMesh("bunny.obj", 0);
        bunny2->Transform(translatebunny2*(scalebunny2*rotatebunny2));
        PhongMaterial* bunmat = new PhongMaterial();
        bunmat->alpha = 30;
        bunmat->Kd = Color(0, 0.7, 0.1);
        bunmat->Ks = Color(0.05, 0.05, 0.05);
            TriangleMesh* bunny3 = new TriangleMesh("bunny.obj", bunmat);
            bunny3->Transform(translatebunny3*(scalebunny3*rotatebunny3));*/



    /*
    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.6f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scale (0.43f, 0, 0, 0,
                    0, 0.43f, 0, 0,
                    0, 0, 0.43f, 0,
                    0, 0, 0, 1);
    TriangleMesh teapot("teapotwithnormals.obj", 0);
    teapot.Transform(translate*scale);*/
    //Renderer* boxrenderer = new RayTracer();
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-Original.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene("trilight.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene());
    Vector3d camPos = Vector3d(0, 1.4, 3);
    Vector3d target = Vector3d(0, 1, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //box.camera = new Camera(Vector3d(0, 1, 0), camPos, camdir);
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, 2.4, 0.05));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));
    
    //box.camera->SetFov(75);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.65, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(20, 200, 50), &box);
    //AreaLight* cubearealight2 = new AreaLight(Vector3d(.25, 0.99, 1.35), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(3, 5, 50), &box);

    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.79, 1.35), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(400, 400, 400), s);
    
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, -10.99, -30), Vector3d(0.3, 0.0, 0), Vector3d(0.0, 0.3, 0), Color(50000, 50000, 50000), s);

    //MeshLight* cubearealight = new MeshLight(Color(50, 50, 50), "trilight.obj");

    /*MeshTriangle* m = new MeshTriangle(Vector3d(-100000, 100000, -3), Vector3d(100000, 100000, -3), Vector3d(0, -100000, -3));
    box.AddShape(m);
    m->material = new PhongMaterial();
    ((PhongMaterial*)m->material)->Kd = Color(0.7, 0.7 , 0.7);*/

    //s->SetEnvironmentLight(ps);
    //AshikhminShirley* mat = new AshikhminShirley;
    PhongMaterial* mat = new PhongMaterial;
    mat->Kd = Color(0.65, 0.65, 0.6);
    mat->Ks = Color(0.08, 0.08, 0.08);
    mat->alpha = 300;
  /*  TriangleMesh* ajax = new TriangleMesh("Ajax_Jotero_com.obj", mat);
    double rt = -0.8;
    Matrix3d rot (cos(rt), 0, sin(rt), 0,
                  0, 1, 0, 0,
                  -sin(rt), 0, cos(rt), 0,
                  0, 0, 0, 0);
    Matrix3d move(1,0,0,0,
                   0,1,0,0,
                   0,0,1,17.28,
                   0,0,0,1);
    Matrix3d scale(0.035f,0,0,0,
                   0,0.035f,0,0,
                   0,0,0.035f,0,
                   0,0,0,0.035f);
    Matrix3d move2(1,0,0,-0.4,
                   0,1,0,-0.7,
                   0,0,1,1.7,
                   0,0,0,0);
    
    ajax->Transform(rot);
    ajax->Transform(move);
    ajax->Transform(scale);
    ajax->Transform(move2);
    s->AddModel(ajax);
*/
    Random ballsR(41);
    //Random test(1);

    //Random ballsC(0);

    for(int i = 0; i < 120; i++) {
        Sphere* sphere = new Sphere(Vector3d(ballsR.GetDouble(-1, 1), ballsR.GetDouble(0, 2), ballsR.GetDouble(-1, 1)), ballsR.GetDouble(0, 0.2));
        auto a = new AshikhminShirley();
        a->Rd = Color(ballsR.GetDouble(0, 1), ballsR.GetDouble(0, 1), ballsR.GetDouble(0, 1));
        a->Rs = Color(ballsR.GetDouble(0, 1-a->Rd.r), ballsR.GetDouble(0, 1-a->Rd.g), ballsR.GetDouble(0, 1-a->Rd.b));
        int asdf = ballsR.GetInt(0, 1) > 2;
        if(asdf > 2)
            a->n = ballsR.GetInt(1, 500);
        else if(asdf > 1)
            a->n = ballsR.GetInt(1, 50);
        else
            a->n = ballsR.GetInt(1, 5);
        sphere->SetMaterial(a);
        s->AddModel(sphere);
    }

    //SphereLight* boxLight = new SphereLight(Vector3d(0.5, 1.1, 0.5), 0.11, Color(500, 500, 500));
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.2, 1.97, -0.2), Vector3d(0.4, 0.0, 0.0), Vector3d(0.0, 0, 0.4), Color(500, 500, 500));

    //auto a = new AshikhminShirley();
    //a->Rs = Color(0.5, 0.4, 0.6);
    //a->Rd = Color(0.3, 0.4, 0.6);
    //a->n = 200;
    //Sphere* sphere = new Sphere(Vector3d(-0.1, 1.1, 0.3), 0.11);
    //sphere->SetMaterial(a);
    //s->AddModel(sphere);
    
    /*auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.5, 0.3), 0.21);
    sphere2->SetMaterial(g);*/
    /*sphere2->AddToScene(*s);
    s->AddModel(sphere2);*/

    //boxLight->AddToScene(s);
    //s->AddLight(boxLight);

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<PathTracer>(new PathTracer(s));

#endif
#ifdef BOX

    /*
    Matrix3d rotatebunny (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny (1, 0, 0, 0,
                        0, 1, 0, -0.0f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny (0.53f, 0, 0, 0,
                    0, 0.53f, 0, 0,
                    0, 0, 0.53f, 0,
                    0, 0, 0, 1);*/
    /*
        Matrix3d rotatebunny2 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny2 (1, 0, 0, 0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny2 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);
        Matrix3d rotatebunny3 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny3 (1, 0, 0, -0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny3 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);*/

    //MeshLight* bunny = new MeshLight(Color(10, 10, 10), "bunny.obj");
    //bunny->Transform(translatebunny*(scalebunny*rotatebunny));
    /*
        TriangleMesh* bunny2 = new TriangleMesh("bunny.obj", 0);
        bunny2->Transform(translatebunny2*(scalebunny2*rotatebunny2));
        PhongMaterial* bunmat = new PhongMaterial();
        bunmat->alpha = 30;
        bunmat->Kd = Color(0, 0.7, 0.1);
        bunmat->Ks = Color(0.05, 0.05, 0.05);
            TriangleMesh* bunny3 = new TriangleMesh("bunny.obj", bunmat);
            bunny3->Transform(translatebunny3*(scalebunny3*rotatebunny3));*/



    /*
    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.6f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scale (0.43f, 0, 0, 0,
                    0, 0.43f, 0, 0,
                    0, 0, 0.43f, 0,
                    0, 0, 0, 1);
    TriangleMesh teapot("teapotwithnormals.obj", 0);
    teapot.Transform(translate*scale);*/
    //Renderer* boxrenderer = new RayTracer();
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-Original-Mats2.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene("trilight.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene());
    Vector3d camPos = Vector3d(0, 1.1, 3);
    Vector3d target = Vector3d(0, 1, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //box.camera = new Camera(Vector3d(0, 1, 0), camPos, camdir);
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, 2.4, 0.05));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));
    
    //box.camera->SetFov(75);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.65, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(20, 200, 50), &box);
    //AreaLight* cubearealight2 = new AreaLight(Vector3d(.25, 0.99, 1.35), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(3, 5, 50), &box);

    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.79, 1.35), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(400, 400, 400), s);
    
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, -10.99, -30), Vector3d(0.3, 0.0, 0), Vector3d(0.0, 0.3, 0), Color(50000, 50000, 50000), s);

    //MeshLight* cubearealight = new MeshLight(Color(50, 50, 50), "trilight.obj");

    /*MeshTriangle* m = new MeshTriangle(Vector3d(-100000, 100000, -3), Vector3d(100000, 100000, -3), Vector3d(0, -100000, -3));
    box.AddShape(m);
    m->material = new PhongMaterial();
    ((PhongMaterial*)m->material)->Kd = Color(0.7, 0.7 , 0.7);*/

    //s->SetEnvironmentLight(ps);
    //AshikhminShirley* mat = new AshikhminShirley;
    PhongMaterial* mat = new PhongMaterial;
    mat->Kd = Color(0.65, 0.65, 0.6);
    mat->Ks = Color(0.08, 0.08, 0.08);
    mat->alpha = 300;
  /*  TriangleMesh* ajax = new TriangleMesh("Ajax_Jotero_com.obj", mat);
    double rt = -0.8;
    Matrix3d rot (cos(rt), 0, sin(rt), 0,
                  0, 1, 0, 0,
                  -sin(rt), 0, cos(rt), 0,
                  0, 0, 0, 0);
    Matrix3d move(1,0,0,0,
                   0,1,0,0,
                   0,0,1,17.28,
                   0,0,0,1);
    Matrix3d scale(0.035f,0,0,0,
                   0,0.035f,0,0,
                   0,0,0.035f,0,
                   0,0,0,0.035f);   
    Matrix3d move2(1,0,0,-0.4,
                   0,1,0,-0.7,
                   0,0,1,1.7,
                   0,0,0,0);
    
    ajax->Transform(rot);
    ajax->Transform(move);
    ajax->Transform(scale);
    ajax->Transform(move2);
    s->AddModel(ajax);
*/
    Randomizer ballsR(47);
    Randomizer test(1);

    Randomizer ballsC(0);

    //s->SetPartitioning(new BrutePartitioning());

    SphereLight* boxLight = new SphereLight(Vector3d(0.5, 1.1, 0.5), 0.11, 100*Color(0.5, 0, 0.8));
    s->AddLight(boxLight);
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.2, 1.97, -0.2), Vector3d(0.4, 0.0, 0.0), Vector3d(0.0, 0, 0.4), Color(500, 500, 500));

    //auto a = new AshikhminShirley();
    //a->Rs = Color(0.5, 0.4, 0.6);
    //a->Rd = Color(0.3, 0.4, 0.6);
    //a->n = 200;
    //Sphere* sphere = new Sphere(Vector3d(-0.1, 1.1, 0.3), 0.11);
    //sphere->SetMaterial(a);
    //s->AddModel(sphere);
    
    /*auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.5, 0.3), 0.21);
    sphere2->SetMaterial(g);*/
    /*sphere2->AddToScene(*s);
    s->AddModel(sphere2);*/

    //boxLight->AddToScene(s);
    s->AddLight(boxLight);

    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
    r = std::shared_ptr<PathTracer>(new PathTracer(s));
#endif
#ifdef MESHLIGHTBOX

    /*
    Matrix3d rotatebunny (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny (1, 0, 0, 0,
                        0, 1, 0, -0.0f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny (0.53f, 0, 0, 0,
                    0, 0.53f, 0, 0,
                    0, 0, 0.53f, 0,
                    0, 0, 0, 1);*/
    /*
        Matrix3d rotatebunny2 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny2 (1, 0, 0, 0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny2 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);
        Matrix3d rotatebunny3 (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);
    Matrix3d translatebunny3 (1, 0, 0, -0.5,
                        0, 1, 0, 0,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scalebunny3 (0.23f, 0, 0, 0,
                    0, 0.23f, 0, 0,
                    0, 0, 0.23f, 0,
                    0, 0, 0, 1);*/

    //MeshLight* bunny = new MeshLight(Color(10, 10, 10), "bunny.obj");
    //bunny->Transform(translatebunny*(scalebunny*rotatebunny));
    /*
        TriangleMesh* bunny2 = new TriangleMesh("bunny.obj", 0);
        bunny2->Transform(translatebunny2*(scalebunny2*rotatebunny2));
        PhongMaterial* bunmat = new PhongMaterial();
        bunmat->alpha = 30;
        bunmat->Kd = Color(0, 0.7, 0.1);
        bunmat->Ks = Color(0.05, 0.05, 0.05);
            TriangleMesh* bunny3 = new TriangleMesh("bunny.obj", bunmat);
            bunny3->Transform(translatebunny3*(scalebunny3*rotatebunny3));*/



    /*
    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.6f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);
    Matrix3d scale (0.43f, 0, 0, 0,
                    0, 0.43f, 0, 0,
                    0, 0, 0.43f, 0,
                    0, 0, 0, 1);
    TriangleMesh teapot("teapotwithnormals.obj", 0);
    teapot.Transform(translate*scale);*/
    //Renderer* boxrenderer = new RayTracer();
    auto s = std::shared_ptr<Scene> (new Scene("CornellBox-MeshLights.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene("trilight.obj"));
    //auto s = std::shared_ptr<Scene> (new Scene());
    Vector3d camPos = Vector3d(0, 1.4, 3);
    Vector3d target = Vector3d(0, 1, 0);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //box.camera = new Camera(Vector3d(0, 1, 0), camPos, camdir);
    //s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, 2.4, 0.05));
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));
    
    //box.camera->SetFov(75);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.65, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(20, 200, 50), &box);
    //AreaLight* cubearealight2 = new AreaLight(Vector3d(.25, 0.99, 1.35), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(3, 5, 50), &box);

    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), s);
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.79, 1.35), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(400, 400, 400), s);
    
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, -10.99, -30), Vector3d(0.3, 0.0, 0), Vector3d(0.0, 0.3, 0), Color(50000, 50000, 50000), s);

    //MeshLight* cubearealight = new MeshLight(Color(50, 50, 50), "trilight.obj");

    /*MeshTriangle* m = new MeshTriangle(Vector3d(-100000, 100000, -3), Vector3d(100000, 100000, -3), Vector3d(0, -100000, -3));
    box.AddShape(m);
    m->material = new PhongMaterial();
    ((PhongMaterial*)m->material)->Kd = Color(0.7, 0.7 , 0.7);*/

    //s->SetEnvironmentLight(ps);
    //AshikhminShirley* mat = new AshikhminShirley;
    PhongMaterial* mat = new PhongMaterial;
    mat->Kd = Color(0.65, 0.65, 0.6);
    mat->Ks = Color(0.08, 0.08, 0.08);
    mat->alpha = 300;
  /*  TriangleMesh* ajax = new TriangleMesh("Ajax_Jotero_com.obj", mat);
    double rt = -0.8;
    Matrix3d rot (cos(rt), 0, sin(rt), 0,
                  0, 1, 0, 0,
                  -sin(rt), 0, cos(rt), 0,
                  0, 0, 0, 0);
    Matrix3d move(1,0,0,0,
                   0,1,0,0,
                   0,0,1,17.28,
                   0,0,0,1);
    Matrix3d scale(0.035f,0,0,0,
                   0,0.035f,0,0,
                   0,0,0.035f,0,
                   0,0,0,0.035f);   
    Matrix3d move2(1,0,0,-0.4,
                   0,1,0,-0.7,
                   0,0,1,1.7,
                   0,0,0,0);
    
    ajax->Transform(rot);
    ajax->Transform(move);
    ajax->Transform(scale);
    ajax->Transform(move2);
    s->AddModel(ajax);
*/
    Random ballsR(47);
    Random test(1);

    Random ballsC(0);

    //s->SetPartitioning(new BrutePartitioning());

    /*SphereLight* boxLight = new SphereLight(Vector3d(0.5, 1.1, 0.5), 0.11, 100*Color(0.5, 0, 0.8));
    s->AddLight(boxLight);*/
    //AreaLight* boxLight = new AreaLight(Vector3d(-0.2, 1.97, -0.2), Vector3d(0.4, 0.0, 0.0), Vector3d(0.0, 0, 0.4), Color(500, 500, 500));

    //auto a = new AshikhminShirley();
    //a->Rs = Color(0.5, 0.4, 0.6);
    //a->Rd = Color(0.3, 0.4, 0.6);
    //a->n = 200;
    //Sphere* sphere = new Sphere(Vector3d(-0.1, 1.1, 0.3), 0.11);
    //sphere->SetMaterial(a);
    //s->AddModel(sphere);
    
    /*auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.5, 0.3), 0.21);
    sphere2->SetMaterial(g);*/
    /*sphere2->AddToScene(*s);
    s->AddModel(sphere2);*/

    //boxLight->AddToScene(s);
    //s->AddLight(boxLight);

    r = std::shared_ptr<BDPT>(new BDPT(s));
    e = std::shared_ptr<MeanEstimator>(new MeanEstimator(XRES, YRES));
#endif
#ifdef BALLBOX

    auto s = std::shared_ptr<Scene>(new Scene());

    LambertianMaterial* sphereLeftMat = new LambertianMaterial();
    sphereLeftMat->Kd = Color(0.7, 0.0, 0.0);

    LambertianMaterial* sphereRightMat = new LambertianMaterial();
    sphereRightMat->Kd = Color(0.0, 0.7, 0.0);

    LambertianMaterial* sphereMat = new LambertianMaterial();
    sphereMat->Kd = Color(0.7, 0.7, 0.7);

    Sphere* leftSphere = new Sphere(Vector3d(-10, 0, 0), 9);
    leftSphere->SetMaterial(sphereLeftMat);

    Sphere* rightSphere = new Sphere(Vector3d(10, 0, 0), 9);
    rightSphere->SetMaterial(sphereRightMat);

    Sphere* topSphere = new Sphere(Vector3d(0, 0, 10), 9);
    topSphere->SetMaterial(sphereMat);

    Sphere* bottomSphere = new Sphere(Vector3d(0, 0, -10), 9);
    bottomSphere ->SetMaterial(sphereMat);

    Sphere* backSphere = new Sphere(Vector3d(0, 10, 0), 9);
    backSphere->SetMaterial(sphereMat);

    Sphere* smallSphere = new Sphere(Vector3d(1, 0, -1), 0.5);
    smallSphere->SetMaterial(sphereMat);

    Sphere* bigSphere = new Sphere(Vector3d(-1, 0, -1), 0.5);
    bigSphere->SetMaterial(sphereMat);

    topSphere->AddToScene(*s);
    leftSphere->AddToScene(*s);
    rightSphere->AddToScene(*s);
    bottomSphere->AddToScene(*s);
    backSphere->AddToScene(*s);
    smallSphere->AddToScene(*s);
    bigSphere->AddToScene(*s);

    SphereLight* sphereLight = new SphereLight(Vector3d(0, 1.5, 0.5), 0.5, Color(500, 500, 500));

    sphereLight->AddToScene(s);

    s->SetCamera(new ThinLensCamera(Vector3d(0, 0, 1.0f), Vector3d(0, -10, 0), Vector3d(0, 1, 0), XRES, YRES, 50, 10, 0.3f));

    r = std::shared_ptr<PathTracer>(new PathTracer(s));


#endif

#ifdef CONFERENCE
    stringstream sstr;

    Vector3d camerapos(28.3f, 3.0f, 5.8f);
    Vector3d cameradir = Vector3d(7.6f, 24.9f, 1.5f) - camerapos;
    Vector3d cameraup(0, 0, 1.0f);

    //RayTracer* confrenderer = new RayTracer();
    //Renderer* confrenderer = new RayTracer();
    Vector3d confcamerapos(28.3f, 3.0f, 5.8f);
    Vector3d confcameradir = Vector3d(7.6f, 24.9f, 1.5f) - confcamerapos;
    confcameradir.Normalize();

    auto s = std::shared_ptr<Scene>(new Scene("conference.obj"));

    auto camera = new PinholeCamera(Vector3d(0, 0, 1.0f), Vector3d(20.3f, 3.0f, 5.8f) + confcameradir * 3, confcameradir, XRES, YRES, 65);
    s->SetCamera(camera);
    //PointLight* conferencelight = new PointLight(Vector3d(19.0f, 11.0f, 8.0f), Color(1000.0f, 1000.0f, 1000.0f), &conference);
    AreaLight* conferencelight = new AreaLight(Vector3d(14.0f, 11.0f, 8.78f), Vector3d(0.0f, 3.0f, 0.0f), Vector3d(3.0f, 0.0f, 0.0f), Color(300000, 300000, 300000), s);
    conferencelight->AddToScene(s);

    r = std::shared_ptr<PathTracer>(new PathTracer(s));

#endif

    #ifdef KITCHEN
    Vector3d camerapos(48.7f, 103.2f, 3.7f);
    Vector3d cameradir = Vector3d(-62.9f, -62.4f, -35.1f) - camerapos;
    //Vector3d camerapos(-52.4, -19.6, 13.7);
    //Vector3d cameradir = Vector3d(-82.0, -95.4, -7.6) - camerapos;
    cameradir.Normalize();
//	Vector3d camerapos(73.1f, 56.4f, -4.f);
//	Vector3d cameradir = Vector3d(0, 0, -15) - camerapos;
    Vector3d cameraup(0, 0, 1.0f);
    //PreethamSky* ps = new PreethamSky(75, 75, 2, 10);

    auto s = std::shared_ptr<Scene> (new Scene("kitchen_clean.obj"));	
    //s->SetEnvironmentLight(ps);
//	r = new PathTracer();
    //r = new BDPT();
    //BDPT* kitchenrenderer = new BDPT();
    //Renderer* kitchenrenderer = new RayTracer();
    Vector3d kitchencamerapos(28.3f, 3.0f, 5.8f);
    //Scene kitchen("kitchen_clean.obj");
    //s = new Scene("kitchen_clean.obj");
    //s->SetRenderer(r);
    s->SetCamera(new ThinLensCamera(Vector3d(0, 0, 1.0f), camerapos, cameradir, XRES, YRES, 50, 150, 0.3f));
    //kitchen.SetCamera(new PinholeCamera(Vector3d(0, 0, 1.0f), camerapos, cameradir, XRES, YRES, 50));
    //PointLight* kitchenlight = new PointLight(Vector3d(0.f, 0.f, 40.0f), Color(10000.0f, 10000.0f, 10000.0f), &kitchen);
//	AreaLight* kitchenlight = new AreaLight(10*Vector3d(130.0f, -300.0f, 130.5f), Vector3d(0.0f, 10*30.0f, 0.0f), Vector3d(10*30.0f, 0.0f, 0.0f), Color(13000, 12500, 11000), s);
    //AreaLight* kitchenlight = new AreaLight(10*Vector3d(130.0f, -300.0f, 130.5f), Vector3d(0.0f, 10*30.0f, 0.0f), Vector3d(10*30.0f, 0.0f, 0.0f), Color(13000, 12500, 11000), s);
    AreaLight* kitchenlight = new AreaLight(10.0f*Vector3d(100.0f, -300.0f, 100.5f), 10*Vector3d(0.0f, 30.0f, 0.0f), 10*Vector3d(30.0f, 0.0f, 0.0f), Color(13000, 12500, 11000), s);
    //AreaLight* kitchenlight = new AreaLight(10*Vector3d(130.0f, -300.0f, 130.5f), Vector3d(0.0f, 10*30.0f, 0.0f), Vector3d(10*30.0f, 0.0f, 0.0f), Color(13000, 12500, 11000), s);
    //AreaLight* kitchenlight2 = new AreaLight(Vector3d(-100000.0f, -100000.0f, 100.5f), Vector3d(0.0f, 200000.0f, 0.0f), Vector3d(200000.0f, 0.0f, 0.0f), Color(0, 0, 300), s);
    kitchenlight->AddPortal(Vector3d(-30, -120, -20), Vector3d(80, 0, 0), Vector3d(0, 0, 60));
    kitchenlight->AddToScene(s);
    //kitchenlight2->AddToScene(*s);
    //timer1 = timer->GetTime();
    r = std::shared_ptr<PathTracer>(new PathTracer(s));
    //r = std::shared_ptr<LightTracer>(new LightTracer(s));
    //r = std::shared_ptr<BDPT>(new BDPT(s));
    //r = std::shared_ptr<RayTracer>(new RayTracer(s));
    r->SetSPP(1);
    //timer1 = timer->GetTime() - timer1;
//	pScene = &kitchen;
#endif
    #ifdef OCLBOX

    //Renderer* boxrenderer = new RayTracer();
    //PathTracer* boxrenderer = new PathTracer();
    //LightTracer* boxrenderer = new LightTracer();
    auto s = std::shared_ptr<Scene> (new Scene("oclbox.obj"));
    Vector3d camPos = Vector3d(0.9f, 0.9f, 2.5f);
    Vector3d target = Vector3d(0, 0.3f, 1.5f);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    s->SetCamera(new PinholeCamera( Vector3d(0, 1, 0), Vector3d(0.0, -0.0001, -2.7232), Vector3d(0, 0, 1.0018), XRES, YRES, 75));
    //AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 0.00), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(2200, 2200, 2200), &box);
    AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.405, 0.00), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(1500, 1500, 1500), s);
    //SphereLight* cubearealight = new SphereLight(Vector3d(-0.8, 0.500, 0.00), 0.1, Color(5000, 5000, 5000));

    cubearealight->AddToScene(s);

    DielectricMaterial* spheremat = new DielectricMaterial();
    //MirrorMaterial* spheremat = new MirrorMaterial();
    spheremat->m_ior = 1.5f;
    Sphere* boxsphere = new Sphere(Vector3d(0.3, -0.30, -0.0), 0.3);
    boxsphere->SetMaterial(spheremat);
    boxsphere->AddToScene(*s);

    Timer buildtime;
    timer1 = buildtime.GetTime();
    //s->Build();
    timer1 = buildtime.GetTime() - timer1;
            //r = std::shared_ptr<LightTracer>(new LightTracer(s));
        //r = std::shared_ptr<PathTracer>(new PathTracer(s));
    //r = std::shared_ptr<BDPT>(new BDPT(s));
    //pScene = &box;
#endif
#ifdef CSGTEST
    auto s = std::shared_ptr<Scene> (new Scene());
    //Vector3d camPos = Vector3d(0.9f, 0.9f, 2.5f);
    //Vector3d camPos = Vector3d(3.0f, 2.0f, 4.5f);
    //Vector3d target = Vector3d(0, 0.0f, 0.0f);
    Vector3d target = Vector3d(0.0f, 0.0f, 4.5f);
    Vector3d camPos = Vector3d(0, 0.0f, 0.0f);
    Vector3d camdir = target-camPos; 
    camdir.Normalize();
    s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75));
    AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.405, 0.00), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(1500, 1500, 1500), s);
    //CsgCuboid* cuboid = new CsgCuboid(target, Vector3d(0, 1, 0), Vector3d(1, 0, 0), 4.0/5, 9.0/5, 1.0/5);
    //CsgCylinder* csgObject = new CsgCylinder(target, Vector3d(0, 0, 1), 2, 0.3);
    csgObject->Rotate(Vector3d(0, 1, 1), 1.35);
    csgObject->Translate(Vector3d(0, 0, 4.5f));

    //   csgObject->Rotate(Vector3d(0, 1, 0), 3.14);
       //csgObject->Rotate(Vector3d(0, 1, 0), 0.44);
    csgObject->AddToScene(*s);
    LambertianMaterial* mat = new LambertianMaterial;
    mat->Kd = Color(0.7, 0.7, 0.7);
    csgObject->SetMaterial(mat);
    r = std::shared_ptr<RayTracer>(new RayTracer(s));


#endif

}