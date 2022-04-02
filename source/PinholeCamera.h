#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H

#include "Vector3d.h"
#include "Camera.h"
#include "Ray.h"
#include <assert.h>

class PinholeCamera : public Camera
{
public:
    PinholeCamera();
	PinholeCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, float fov);
	~PinholeCamera();
	
	Ray GetRayFromPixel(int x, int y, float a, float b, float u, float v) const;
	bool GetPixelFromRay(const Ray& ray, int& x, int& y, float u, float v) const;

    void SetFov(float fov);
    void SampleAperture(Vector3d& pos, float& u, float& v) const;

	float GetPixelArea() const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
};

#endif