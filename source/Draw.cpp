#include "CsgIntersection.h"
#include "CsgSphere.h"
#include "CsgCylinder.h"
#include "CsgCuboid.h"
#include "Random.h"
#include "LightTracer.h"
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
#include "Texture.h"
#include "Cubemap.h"
#include "triangle.h"
#include "kdtree.h"
#include "trianglemesh.h"
#include "Scene.h"
#include "RayTracer.h"
#include "PathTracer.h"
#include "AreaLight.h"
#include "PointLight.h"
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
#include "PreethamSky.h"
#include "MeshLight.h"

KDTree* tree;
Texture* normalmap;
Texture* test;
Texture* bl;
Cubemap* cubemap;

#define BALLSBOX
//#define CONFERENCE
// #define BALLBOX
//#define LEGOCAR
//#define KITCHEN
//#define EMPTYBOX
//#define DODGE
//#define TEAPOT
//#define OCLBOX
//#define SHINYBALL
//#define CSGTEST
#define SAMPLES_PER_PIXEL 1

void MakeScene(std::shared_ptr<Renderer>& r)
{
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
    s->SetCamera(new ThinLensCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 75, 2.4, 0.05));
    //s->SetCamera(new PinholeCamera(Vector3d(0, 1, 0), camPos, camdir, XRES, YRES, 65));
	
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
    float rt = -0.8;
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

    //AreaLight* cubearealight = new AreaLight(Vector3d(-0.3, 1.5, -0.3), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(5, 5, 5), s);
    SphereLight* cubearealight = new SphereLight(Vector3d(0.5, 1.1, 0.5), 0.11, Color(500, 500, 500));

    auto a = new AshikhminShirley();
    a->Rs = Color(0.5, 0.4, 0.6);
    a->Rd = Color(0.3, 0.4, 0.6);
    a->n = 200;
    Sphere* sphere = new Sphere(Vector3d(-0.1, 1.1, 0.3), 0.11);
    sphere->SetMaterial(a);
    sphere->AddToScene(*s);
    
    auto g = new DielectricMaterial();
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, 1.8, 0.3), 0.11);
    sphere2->SetMaterial(g);
    sphere2->AddToScene(*s);

    /*LambertianMaterial* sphereMat = new LambertianMaterial();
    LambertianMaterial* sphereMat2 = new LambertianMaterial();
    sphereMat->Kd = Color(0.9, 0.9, 0.9);
    sphereMat2->Kd = Color(0.9, 0.4, 0.3);
    Sphere* sphere = new Sphere(Vector3d(0.5, -0.5, 1.8), 0.5);
    Sphere* sphere2 = new Sphere(Vector3d(-0.1, -0.9, 1.5), 0.2);
    sphere->SetMaterial(sphereMat);
    sphere2->SetMaterial(sphereMat2);
    sphere->AddToScene(*s);
    sphere2->AddToScene(*s);*/
    cubearealight->AddToScene(s);
    
    r = std::shared_ptr<LightTracer>(new LightTracer(s));

    //LambertianMaterial* csgMat1 = new LambertianMaterial;
    //csgMat1->Kd = Color(0.7, 0.7, 0.7);
    /*
	for(int i = 0; i < 31; i++)
	{
		PhongMaterial* spheremat = new PhongMaterial();
		spheremat->Kd = Color(ballsR.GetFloat(0, 0.8), ballsR.GetFloat(0, 0.8), ballsR.GetFloat(0, 0.8));//Color(ballsR.GetFloat(0,1), ballsR.GetFloat(0,1), ballsR.GetFloat(0, 1));
        float reflcol = ballsR.GetFloat(0, 0.1);
        spheremat->Ks = Color(reflcol, reflcol, reflcol);
        spheremat->alpha = exp(ballsC.GetFloat(0, 6));
		//MirrorMaterial* spheremat = new MirrorMaterial();

        //AshikhminShirley* spheremat = new AshikhminShirley;
//        if(test.GetFloat(0, 2) > 1)
//            spheremat->Rd = Color(0.6667, 0.4745, 0.2706);
//        else
//            spheremat->Rd = Color(0.3490, 0.3490, 0.3490);
//        spheremat->Rs = Color(0.02, 0.02, 0.02);
//        spheremat->n = 400;

		Sphere* boxsphere = new Sphere(Vector3d(ballsR.GetFloat(-0.8,0.8), ballsR.GetFloat(-0.8,0.8), 2+ballsR.GetFloat(-0.8, 0.8)), 0.12);
				//if(ballsR.GetFloat(0,1) < 0.85)

        boxsphere->SetMaterial(spheremat);
                //else
               // {
               //     boxsphere->SetMaterial(new DielectricMaterial());
               //     boxsphere->SetRadius(0.20);
               // }
		s->AddModel(boxsphere);
	}*/
    //SphereLight* cubearealight = new SphereLight(Vector3d(-0.4, -.3, 1.5), 0.1, Color(186, 186, 186));
    //SphereLight* cubearealight2 = new SphereLight(Vector3d(0.7, -.6, 2.3), 0.1, Color(186, 186, 186));
    //MeshTriangle* tri = new MeshTriangle(Vector3d(-30000, -0.7, -40000), Vector3d(-30000, -0.7, 30000), Vector3d(30000, -0.7, 30000));
    //tri->AddToScene(box);


    //Triangle* t = new Triangle(Vertex3d(Vector3d(-0.4, -0.4, 2), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)), Vertex3d(Vector3d(-0.4, -0.4, 1.5), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)), Vertex3d(Vector3d(0.4, -0.5, 1.5), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)));
    //Triangle* t = new Triangle(Vector3d(-0.4, 0.6, 2), Vector3d(-0.4, 0.6, 1.5), Vector3d(0.4, 0.4, 1.5));
    //Triangle* t = new Triangle(Vector3d(-0.4, 0.6, 1.5), Vector3d(-0.4, 0.6, 2), Vector3d(0.4, 0.4, 1.5));

    //Triangle* t = new Triangle(Vertex3d(Vector3d(-0.4, 0.6, 2), Vector3d(1, 0, 0), Vector2d(0, 0)), Vertex3d(Vector3d(-0.4, 0.6, 1.5), Vector3d(1, 0, 0), Vector2d(0, 0)), Vertex3d(Vector3d(0.4, 0.4, 1.5), Vector3d(1, 0, 0), Vector2d(0, 0)));
    //Triangle* t = new Triangle(Vector3d(-0.4, 0.6, 1.5), Vector3d(-0.4, 0.6, 2), Vector3d(0.4, 0.4, 1.5));
    //Triangle* t = new Triangle(Vector3d(-0.4, 0.6, 2), Vector3d(-0.4, 0.6, 1.5), Vector3d(0.4, 0.4, 1.5));
    //LambertianMaterial* m = new LambertianMaterial();
    //m->Kd = Color(0, 1, 1);
    //m->alpha = 100;

    //Triangle* t = new Triangle(Vertex3d(Vector3d(-0.4, 0.6, 2), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)), Vertex3d(Vector3d(-0.4, 0.6, 1.5), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)), Vertex3d(Vector3d(0.4, 0.4, 1.5), Vector3d(1/sqrtf(2), 1/sqrtf(2), 0), Vector2d(0, 0)));


    PhongMaterial* m = new PhongMaterial();
    m->alpha = 100;
    m->Ks = Color(1, 1, 0);
    m->Kd = Color(0, 1 ,1);

    /*AshikhminShirley* m = new AshikhminShirley();
    m->n = 100;
    //m->n = 0;
    m->Rs = Color(1, 1, 0);
    m->Rd = Color(0, 1, 1);*/



 //   t->SetMaterial(m);
 //   t->AddToScene(*s);

    //SphereLight* sl = new SphereLight(Vector3d(0.7, 0.7, 2.4), 0.15, Color(300, 300, 300));
    //sl->AddToScene(s);
	cubearealight->AddToScene(s);
        //bunny->AddToScene(s);
    //    cubearealight2->AddToScene(s);
     //   teapot.AddToScene(*s);

                    //bunny2->AddToScene(*s);
            //bunny3->AddToScene(*s);

    //cubearealight2->AddToScene(box);
    //cubearealight->AddPortal(Vector3d(-1.5, -1.5, 1.1), Vector3d(3, 0, 0), Vector3d(0, 3, 0));
    //Sphere* transsphere = new Sphere(Vector3d(-0.9, -0.7, 1.3), 0.26);
    //Sphere* transsphere = new Sphere(Vector3d(-0.0, -0.5, 1.5), 0.26);
    //DielectricMaterial* transmat = new DielectricMaterial();
    //transmat->m_ior = 1.5f;
    //AshikhminShirley* transmat = new AshikhminShirley;
    //PhongMaterial* transmat = new PhongMaterial;
    //transmat->Kd = Color(0.05f, 0.05f, 0.05f);
    //transmat->Ks = Color(0.8, 0.8, 0.8);
    //transmat->alpha = 800;
    //tri->material = transmat;
    /*PhongMaterial* transmat = new PhongMaterial;
    transmat->Kd = Color(0.5, 0.05f, 0.05f);
    transmat->Ks = Color(0.05, 0.05, 0.05);
    transmat->alpha = 50;*/
    //LambertianMaterial* lm = new LambertianMaterial;
    //lm->Kd = Color(0.7, 0.7, 0.7);
    //transsphere->SetMaterial(transmat);

    //r = std::shared_ptr<LightTracer>(new LightTracer(s));
    //r = std::shared_ptr<PathTracer>(new PathTracer(s));
    //r = std::shared_ptr<RayTracer>(new RayTracer(s));
    //r = std::shared_ptr<BDPT>(new BDPT(s));
	//r = new BDPT();
	r->SetSPP(1);
    //transsphere->AddToScene(box);
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
    r = std::shared_ptr<PathTracer>(new PathTracer(s));
    auto camera = new PinholeCamera(Vector3d(0, 0, 1.0f), Vector3d(28.3f, 3.0f, 5.8f) + confcameradir * 3, confcameradir, XRES, YRES, 65);
    s->SetCamera(camera);
    //PointLight* conferencelight = new PointLight(Vector3d(19.0f, 11.0f, 8.0f), Color(1000.0f, 1000.0f, 1000.0f), &conference);
    AreaLight* conferencelight = new AreaLight(Vector3d(14.0f, 11.0f, 8.78f), Vector3d(0.0f, 3.0f, 0.0f), Vector3d(3.0f, 0.0f, 0.0f), Color(300, 300, 300), s);
    conferencelight->AddToScene(s);

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
/*
void Draw(Gfx* g, ColorBuffer& cb)
{
	setuptime = timer->GetTime();

	const float xres = (float)g->GetXRes();
	const float yres = (float)g->GetYRes();

	Scene* pScene;

	
#ifdef CONFERENCE
	stringstream sstr;
	
	Vector3d camerapos(28.3f, 3.0f, 5.8f);
	Vector3d cameradir = Vector3d(7.6f, 24.9f, 1.5f) - camerapos;
	Vector3d cameraup(0, 0, 1.0f);

	//PathTracer* confrenderer = new PathTracer();
	LightTracer* confrenderer = new LightTracer();
	//RayTracer* confrenderer = new RayTracer();
	confrenderer->SetSPP(SAMPLES_PER_PIXEL);
	//Renderer* confrenderer = new RayTracer();
	Vector3d confcamerapos(28.3f, 3.0f, 5.8f);
	Vector3d confcameradir = Vector3d(7.6f, 24.9f, 1.5f) - confcamerapos;
	confcameradir.Normalize();
	Scene conference("conference.obj");
	conference.SetRenderer(confrenderer);
	conference.SetCamera(new PinholeCamera(Vector3d(0, 0, 1.0f), Vector3d(28.3f, 3.0f, 5.8f) + confcameradir*3, confcameradir, 65));
	//PointLight* conferencelight = new PointLight(Vector3d(19.0f, 11.0f, 8.0f), Color(1000.0f, 1000.0f, 1000.0f), &conference);
	AreaLight* conferencelight = new AreaLight(Vector3d(14.0f, 11.0f, 8.78f), Vector3d(0.0f, 3.0f, 0.0f), Vector3d(3.0f, 0.0f, 0.0f), Color(300, 300, 300), &conference);
	conferencelight->AddToScene(conference);

	timer1 = timer->GetTime();
	conference.Build();
	timer1 = timer->GetTime() - timer1;
	pScene = &conference;
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
	
	PathTracer* kitchenrenderer = new PathTracer();
	//BDPT* kitchenrenderer = new BDPT();
	kitchenrenderer->SetSPP(100);
	//Renderer* kitchenrenderer = new RayTracer();
    Vector3d kitchencamerapos(28.3f, 3.0f, 5.8f);
	Scene kitchen("kitchen_clean.obj");
	kitchen.SetRenderer(kitchenrenderer);
	kitchen.SetCamera(new ThinLensCamera(Vector3d(0, 0, 1.0f), camerapos, cameradir, XRES, YRES, 50, 150, 0.3f));
    //kitchen.SetCamera(new PinholeCamera(Vector3d(0, 0, 1.0f), camerapos, cameradir, XRES, YRES, 50));
	//PointLight* kitchenlight = new PointLight(Vector3d(0.f, 0.f, 40.0f), Color(10000.0f, 10000.0f, 10000.0f), &kitchen);
	AreaLight* kitchenlight = new AreaLight(10*Vector3d(130.0f, -300.0f, 130.5f), Vector3d(0.0f, 10*30.0f, 0.0f), Vector3d(10*30.0f, 0.0f, 0.0f), Color(13000, 12500, 11000), &kitchen);
//	AreaLight* kitchenlight2 = new AreaLight(Vector3d(-100000.0f, -100000.0f, 100.5f), Vector3d(0.0f, 200000.0f, 0.0f), Vector3d(200000.0f, 0.0f, 0.0f), Color(0, 0, 300), &kitchen);
	kitchenlight->AddToScene(kitchen);
//	kitchenlight2->AddToScene(kitchen);
	timer1 = timer->GetTime();
	kitchen.Build();
	timer1 = timer->GetTime() - timer1;
	pScene = &kitchen;
#endif

#ifdef EMPTYBOX

    
    // TEAPOT
    //BDPT* boxrenderer = new BDPT();
	//Renderer* boxrenderer = new RayTracer();
	PathTracer* boxrenderer = new PathTracer();
	//LightTracer* boxrenderer = new LightTracer();
	//boxrenderer->SetSPP(SAMPLES_PER_PIXEL);
    boxrenderer->SetSPP(1);

    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.6f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);

    Matrix3d scale (0.43f, 0, 0, 0,
                    0, 0.43f, 0, 0,
                    0, 0, 0.43f, 0,
                    0, 0, 0, 1);
	Scene box("cube.obj");
	box.SetRenderer(boxrenderer);
	box.SetCamera(new PinholeCamera( Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -0.7232), Vector3d(0, 0, 1.0018), XRES, YRES, 75));
	AreaLight* cubearealight = new AreaLight(Vector3d(-0.3, 0.99, 1.2), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(100, 100, 100), &box);
    TriangleMesh teapot("teapotwithnormals.obj");
    teapot.Transform(translate*scale);
    teapot.AddToScene(box);
    //MeshTriangle t(Vector3d(-0.3f, 0.0f, 1.5f), Vector3d(0.3f, 0.0f, 1.5f), Vector3d(0.3f, -0.4f, 1.5f));
    //t.AddToScene(box);
    PhongMaterial* spheremat = new PhongMaterial();
    //spheremat->Kd = Color(0.7, 0.7, 0.7);//Color(ballsR.GetFloat(0,1), ballsR.GetFloat(0,1), ballsR.GetFloat(0, 1));
		
	//	t.material = spheremat;

      //                  MirrorMaterial* mirmat = new MirrorMaterial();

    //t1.material = mat;
             //   t.material = mirmat;

        Triangle t1(Vector3d(-0.1, 0.8, 1.8), Vector3d(0.1, 0.2, 1.5), Vector3d(-0.1, 0.8, 1.2));
    t1.v0.normal = t1.v1.normal = t1.v2.normal = Vector3d(0, 1, 0);
    Triangle t2(Vector3d(-0.1, 0.75, 1.8), Vector3d(-0.1, 0.75, 1.2), Vector3d(0.1, 0.15, 1.5));
    //t2.v0->normal = t2.v1->normal = t2.v2->normal = Vector3d(-1, 0, 0);
    t2.v0.normal = Vector3d(-1, -1, 0);
    t2.v0.normal.Normalize();
    t2.v1.normal = t2.v2.normal = t2.v0.normal;

    DielectricMaterial* mat = new DielectricMaterial();
    mat->m_ior = 1.5f;
    //PhongMaterial* tm = new PhongMaterial();
    //tm->Kd = Color(0.7, 0.7, 0.7);


    t1.SetMaterial(mat);
                //t1.material = mirmat;
    t2.SetMaterial(mat);

    //box.AddModel(&t1);
    //box.AddModel(&t2);

    //Sphere b(Vector3d(0, -0.5f, 1.5), 0.3f);
    //PhongMaterial* spheremat = new PhongMaterial();
		//spheremat->Kd = Color(0.7, 0.7, 0.7);//Color(ballsR.GetFloat(0,1), ballsR.GetFloat(0,1), ballsR.GetFloat(0, 1));
		
		//b.material = spheremat;
    //b.AddToScene(box);
    
    
    /*
  
    
    // BUNNY
    //BDPT* boxrenderer = new BDPT();
	//Renderer* boxrenderer = new RayTracer();
	PathTracer* boxrenderer = new PathTracer();
	//LightTracer* boxrenderer = new LightTracer();
	//boxrenderer->SetSPP(SAMPLES_PER_PIXEL);
    boxrenderer->SetSPP(100);

    Matrix3d rotate (-1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, -1, 0,
                     0, 0, 0, 1);

    Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, -0.3f,
                        0, 0, 1, 1.6f,
                        0, 0, 0, 1);

    Matrix3d scale (0.53f, 0, 0, 0,
                    0, 0.53f, 0, 0,
                    0, 0, 0.53f, 0,
                    0, 0, 0, 1);
	Scene box("cube.obj");
	box.SetRenderer(boxrenderer);
	box.camera = new Camera( Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -0.3232), Vector3d(0, 0, 1.0018));
	AreaLight* cubearealight = new AreaLight(Vector3d(-0.3, 0.99, 1.2), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(100, 100, 100), &box);
    TriangleMesh teapot("bunny.obj");
    teapot.Transform(translate*(scale*rotate));
    teapot.AddToScene(box);

    MeshTriangle t1(Vector3d(-0.1, 0.8, 1.8), Vector3d(-0.1, 0.8, 1.2), Vector3d(0.1, 0.2, 1.5));
    t1.v0->normal = t1.v1->normal = t1.v2->normal = Vector3d(0, 1, 0);
    MeshTriangle t2(Vector3d(-0.1, 0.75, 1.8), Vector3d(-0.1, 0.75, 1.2), Vector3d(0.1, 0.15, 1.5));
    //t2.v0->normal = t2.v1->normal = t2.v2->normal = Vector3d(-1, 0, 0);
    t2.v0->normal = Vector3d(-1, -1, 0);
    t2.v0->normal.Normalize();
    t2.v1->normal = t2.v2->normal = t2.v0->normal;

    //PhongMaterial* tm = new PhongMaterial();
    //tm->Kd = Color(0.7, 0.7, 0.7);
    DielectricMaterial* tm = new DielectricMaterial();
    tm->m_ior = 1.5;
    t1.material = tm;
    t2.material = tm;

    //box.AddShape(&t1);
    //box.AddShape(&t2);

    
    DielectricMaterial* spheremat = new DielectricMaterial();
    //MirrorMaterial* spheremat = new MirrorMaterial();
	spheremat->m_ior = 1.5f;
	Sphere* boxsphere = new Sphere(Vector3d(0.5, -0.57, 1.5), 0.3);
	boxsphere->material = spheremat;
	//box.AddShape(boxsphere);
    
    

	cubearealight->AddToScene(box);
	Timer buildtime;
	timer1 = buildtime.GetTime();
	box.Build();
	timer1 = buildtime.GetTime() - timer1;
	pScene = &box;
#endif

	
	#ifdef BALLSBOX
	//Renderer* boxrenderer = new RayTracer();
	//PathTracer* boxrenderer = new PathTracer();
	//LightTracer* boxrenderer = new LightTracer();
	//BDPT* boxrenderer = new BDPT();
	boxrenderer->SetSPP(1);
	Scene box("cube.obj");
	//box.SetRenderer(boxrenderer);
    Vector3d camPos = Vector3d(0.9f, 0.9f, 2.5f);
    Vector3d target = Vector3d(0, 0.3f, 1.5f);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
    //box.camera = new Camera(Vector3d(0, 1, 0), camPos, camdir);
    box.SetCamera(new PinholeCamera(Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -0.75), Vector3d(0, 0, 1.0018), XRES, YRES, 75));
	//box.camera = new ThinLensCamera( Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -0.75), Vector3d(0, 0, 1.0018), XRES, YRES, 75, 2.47, 0.05);
    //box.camera->SetFov(75);
	//AreaLight* cubearealight = new AreaLight(Vector3d(-.65, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(20, 200, 50), &box);
    //AreaLight* cubearealight2 = new AreaLight(Vector3d(.25, 0.99, 1.35), Vector3d(0.6, 0, 0), Vector3d(0, 0, 0.6), Color(3, 5, 50), &box);

	AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 1.35), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(400, 400, 400), &box);
	
	/*MeshTriangle* m = new MeshTriangle(Vector3d(-100000, 100000, -3), Vector3d(100000, 100000, -3), Vector3d(0, -100000, -3));
	box.AddShape(m);
	m->material = new PhongMaterial();
	((PhongMaterial*)m->material)->Kd = Color(0.7, 0.7 , 0.7);

	Random ballsR(14);
    Random test(1);
    
	for(int i = 0; i < 40; i++)
	{
		PhongMaterial* spheremat = new PhongMaterial();
		spheremat->Kd = Color(0.7, 0.7, 0.7);//Color(ballsR.GetFloat(0,1), ballsR.GetFloat(0,1), ballsR.GetFloat(0, 1));
		//MirrorMaterial* spheremat = new MirrorMaterial();

        //AshikhminShirley* spheremat = new AshikhminShirley;
//        if(test.GetFloat(0, 2) > 1)
//            spheremat->Rd = Color(0.6667, 0.4745, 0.2706);
//        else
//            spheremat->Rd = Color(0.3490, 0.3490, 0.3490);
//        spheremat->Rs = Color(0.02, 0.02, 0.02);
//        spheremat->n = 400;

		Sphere* boxsphere = new Sphere(Vector3d(ballsR.GetFloat(-0.7,0.7), ballsR.GetFloat(-0.7,0.6), 1.5+ballsR.GetFloat(-0.7, 0.7)), 0.12);
		boxsphere->SetMaterial(spheremat);
		box.AddModel(boxsphere);
	}
    //MeshTriangle* tri = new MeshTriangle(Vector3d(-30000, -0.7, -40000), Vector3d(-30000, -0.7, 30000), Vector3d(30000, -0.7, 30000));
    //tri->AddToScene(box);
	cubearealight->AddToScene(box);
    //cubearealight2->AddToScene(box);

    Sphere* transsphere = new Sphere(Vector3d(-0.9, -0.7, 1.3), 0.26);
    //Sphere* transsphere = new Sphere(Vector3d(-0.0, -0.5, 1.5), 0.26);
    //DielectricMaterial* transmat = new DielectricMaterial();
    //transmat->m_ior = 1.5f;
    //AshikhminShirley* transmat = new AshikhminShirley;
    PhongMaterial* transmat = new PhongMaterial;
    transmat->Kd = Color(0.05f, 0.05f, 0.05f);
    transmat->Ks = Color(0.8, 0.8, 0.8);
    transmat->alpha = 800;
    //tri->material = transmat;
    /*PhongMaterial* transmat = new PhongMaterial;
    transmat->Kd = Color(0.5, 0.05f, 0.05f);
    transmat->Ks = Color(0.05, 0.05, 0.05);
    transmat->alpha = 50;
    //LambertianMaterial* lm = new LambertianMaterial;
    //lm->Kd = Color(0.7, 0.7, 0.7);
    transsphere->SetMaterial(transmat);
    //transsphere->AddToScene(box);

	Timer buildtime;
	timer1 = buildtime.GetTime();
	box.Build();
	timer1 = buildtime.GetTime() - timer1;
	pScene = &box;
#endif
    
#ifdef DEFAULTBOX
	//BDPT* renderer = new BDPT();
	PathTracer* renderer = new PathTracer();
	//RayTracer* renderer = new RayTracer();
	//PathTracer* renderer = new PathTracer();
	renderer->SetSPP(1);
	Scene defaultbox("cornell-box_default.obj");
	defaultbox.SetRenderer(renderer);
	//defaultbox.SetCamera(new ThinLensCamera( Vector3d(0, 1, 0), Vector3d(-2.75f, 2.5f, 3.2f), Vector3d(0, 0, -1.0002f), XRES, YRES, 75, 5.1, 0.2f)); 
    defaultbox.SetCamera(new PinholeCamera( Vector3d(0, 1, 0), Vector3d(-2.75f, 2.5f, 3.2f), Vector3d(0, 0, -1.0002f), xres, yres, 75));
	//PointLight* boxlight = new PointLight(Vector3d(0, 36, 5), Color(2*314.159, 2*314.159, 2*314.149), &defaultbox);
	AreaLight* boxarealight = new AreaLight(Vector3d(-2.2f, 5.45f, -1.7f), Vector3d(-1, 0, 0), Vector3d(0, 0, -1), Color(300, 300, 300), &defaultbox);
	boxarealight->AddToScene(defaultbox);

	Sphere sphere1(Vector3d(-2.0f, 2.5f, -1.9f), 0.3f);

    AshikhminShirley* transmat = new AshikhminShirley;
    transmat->Rd = Color(0.05f, 0.05f, 0.05f);
    transmat->Rs = Color(0.8, 0.8, 0.8);
    transmat->n = 800;

	PhongMaterial* sphere1mat = new PhongMaterial();
	sphere1mat->Kd = Color(0, 0, 0.8f);
	sphere1mat->Ks = Color(0.05f, 0.05f, 0.05f);
	sphere1mat->alpha = 50;
	sphere1.SetMaterial(transmat);
	defaultbox.AddModel(&sphere1);

    //TriangleMesh bun("bunny.obj");
    //bun.Transform(Matrix3d(
//    bun.AddToScene(defaultbox);

    Scene testScene;
    Bytestream bt;
    defaultbox.Save(bt);
    testScene.Load(bt);
    PathTracer* testRenderer = new PathTracer();
    testRenderer->SetSPP(1);
    testScene.SetRenderer(testRenderer);

    //testScene.SetCamera(new PinholeCamera( Vector3d(0, 1, 0), Vector3d(-2.75f, 2.5f, 3.2f), Vector3d(0, 0, -1.0002f), xres, yres, 75));
    //boxarealight->AddToScene(testScene);
    //defaultbox.Build();
	testScene.Build();
    //pScene = &defaultbox;
	pScene = &testScene;
#endif
	
#ifdef LEGOCAR
	Vector3d legocameradir(-2.5f, -1.0f, -1.0f);
	legocameradir.Normalize();
	//PathTracer* renderer = new PathTracer();
	RayTracer* renderer = new RayTracer();
	renderer->SetSPP(SAMPLES_PER_PIXEL);
	Scene legocar("legocar.obj");
	legocar.SetRenderer(renderer);
	legocar.camera = new Camera(Vector3d(0, 0, 1), Vector3d(75.0f, 75.5f, 55.5f), legocameradir);
	AreaLight* legolight = new AreaLight(Vector3d(60, 100, 90), Vector3d(40.0, 0, 0), Vector3d(40, 0, 10.0), Color(180.4f, 180.4f, 180.4f), &legocar);
	//PointLight* legolight = new PointLight(Vector3d(60, 100, 90), Color(10000.0f, 10000.0f, 10000.0f), &legocar);
	legolight->AddToScene(legocar);
	timer1 = timer->GetTime();
	legocar.Build();
	timer1 = timer->GetTime() - timer1;
	pScene = &legocar;
	
	//Color c = renderer->TraceRay(legocar.GetRayFromCoordinate(xres, yres, 375, 305));
//	Color c = renderer->TraceRay(legocar.GetRayFromCoordinate(xres, yres, 375, 305));
#endif

#ifdef DODGE
	Vector3d dodgecamerapos(0.7f, 0.3f, 0.7f);
	dodgecamerapos *= 0.7f;
	Vector3d dodgecameradir = Vector3d(0, 0, 0) - dodgecamerapos;
	dodgecameradir.Normalize();
	//RayTracer* dodgeraytracer = new RayTracer();
	PathTracer* renderer = new PathTracer();
	renderer->SetSPP(SAMPLES_PER_PIXEL);
	Scene dodge("dwc51.obj");
	dodge.SetRenderer(renderer);
	dodge.camera = new Camera(Vector3d(0, 1, 0), dodgecamerapos, dodgecameradir);

	//AreaLight* dodgelight = new AreaLight(Vector3d(15, 10, 10), Vector3d(3, 0, 0), Vector3d(0, 0, 3), Color(5000, 5000, 5000), pScene);

	AreaLight* dodgelight = new AreaLight(Vector3d(0.25, 0.05, 0.05), Vector3d(0, 0, 0.05), Vector3d(0, 0.05, 0), Color(700, 700, 700), pScene);


	dodgelight->AddToScene(dodge);


	//PointLight* dodgelight = new PointLight(Vector3d(15, 15, 15), Color(1000.0f, 1000.0f, 1000.0f), &dodge);
	//dodgelight->AddToScene(dodge);
	timer1 = timer->GetTime();
	dodge.Build();
	timer1 = timer->GetTime() - timer1;
	pScene = &dodge;
#endif
	
#ifdef TEAPOT
	RayTracer* teapotrenderer = new RayTracer();
	Scene teapot("teapotwithnormals.obj");
	teapot.SetRenderer(teapotrenderer);
	Vector3d dir(1.0f, 0.5f, -5.5f);
	dir.Normalize();
	teapot.camera = new Camera(Vector3d(0, 1, 0), Vector3d(-1.0f, 1.0f, -3.5f), -dir);
	//PointLight* teapotlight = new PointLight(Vector3d(4.0f, -0.5f, 8.5f), Color(50.f, 50.f, 50.f), &teapot);
	//teapotlight->AddToScene(teapot);

        Matrix3d translate (1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, -1.5f,
                        0, 0, 0, 1);
    Matrix3d scale (0.7f, 0, 0, 0,
                    0, 0.7f, 0, 0,
                    0, 0, 0.7f, 0,
                    0, 0, 0, 1);
	//Scene box("cube.obj");
	TriangleMesh teapot2("teapotwithnormals.obj");
    teapot2.Transform(translate*scale);
    teapot2.AddToScene(teapot);

	teapot.Build();
	pScene = &teapot;
#endif
	
#ifdef BUNNY
	Scene* pScene;
	Vector3d bunnydir = Vector3d(0, 0, 0) - Vector3d(-0.3f, 0.2f, 0.3f);
	bunnydir.Normalize();
	RayTracer* bunnyraytracer = new RayTracer();
	Scene bunny("bunny2.obj");
	bunny.SetRenderer(bunnyraytracer);
	bunny.camera = new Camera(Vector3d(0, 1, 0), Vector3d(-0.2f, 0.2f, 0.3f), bunnydir);
	//PointLight* legolight = new PointLight(Vector3d(60, 100, 90), Color(10000.0f, 10000.0f, 10000.0f), &legocar);
	//legolight->AddToScene(legocar);
	timer1 = timer->GetTime();
	bunny.Build();
	timer1 = timer->GetTime() - timer1;
	pScene = &bunny;
#endif
    /*
#ifdef OCLBOX

    //Renderer* boxrenderer = new RayTracer();
	//PathTracer* boxrenderer = new PathTracer();
	//LightTracer* boxrenderer = new LightTracer();
	BDPT* boxrenderer = new BDPT();
	boxrenderer->SetSPP(1);
	Scene box("oclbox.obj");
	box.SetRenderer(boxrenderer);
    Vector3d camPos = Vector3d(0.9f, 0.9f, 2.5f);
    Vector3d target = Vector3d(0, 0.3f, 1.5f);
    Vector3d camdir = target-camPos;
    camdir.Normalize();
	box.camera = new Camera( Vector3d(0, 1, 0), Vector3d(0.0, 0.0001, -2.4232), Vector3d(0, 0, 1.0018));
	//AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.99, 0.00), Vector3d(0.3, 0, 0), Vector3d(0, 0, 0.3), Color(2200, 2200, 2200), &box);
    AreaLight* cubearealight = new AreaLight(Vector3d(-.15, 0.405, 0.00), Vector3d(0, 0, 0.3), Vector3d(0.3, 0, 0), Color(800, 800, 800), &box);

	cubearealight->AddToScene(box);

    DielectricMaterial* spheremat = new DielectricMaterial();
    //MirrorMaterial* spheremat = new MirrorMaterial();
	spheremat->m_ior = 1.5f;
	Sphere* boxsphere = new Sphere(Vector3d(0.3, -0.30, -0.0), 0.3);
	boxsphere->material = spheremat;
    boxsphere->AddToScene(box);

	Timer buildtime;
	timer1 = buildtime.GetTime();
	box.Build();
	timer1 = buildtime.GetTime() - timer1;
	pScene = &box;

  
#endif

	logger.File("Starting the frame");

	timer2 = timer->GetTime();

	//WaitForSingleObject(workingMutex, INFINITE);
	if(!g_quitting)
		pScene->Render(cb);
	//ReleaseMutex(workingMutex);
	timer2 = timer->GetTime() - timer2;
	delete pScene->GetRenderer();
	rendertime+=timer->GetTime();
}*/