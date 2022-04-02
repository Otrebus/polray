#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "ddraw.h"
#include "main.h"
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include "color.h"

using namespace std;

class Texture
{
public:
	Texture(const wstring& file);
	Texture(int, int);
	~Texture();
	Texture(Texture&);
	Color GetTexel(int x, int y) const;
	void SetTexel(int x, int y, int color);
	Color GetTexelBLInterp(float x, float y) const;

	int GetHeight() const;
	int GetWidth() const;
	static HINSTANCE hInstance;

protected:
	int*		m_texture;
	int			m_height, m_width;
	float       m_fHeight, m_fWidth;
};

#endif