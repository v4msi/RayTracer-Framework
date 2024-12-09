#pragma once
#include "Basic_Primitive.h"
#include "Triangle.h"
#include "../common/image_volume.h"
#include <string>

class Wall :public Basic_Primitive
{
public:
	Wall(M3DVector3f left_up, M3DVector3f right_up, M3DVector3f right_down, M3DVector3f left_down, M3DVector3f color)
		:Basic_Primitive(_k_wall)
		, _tr1(left_up, right_up, left_down)
		, _tr2(right_up, right_down, left_down)
		, _texture(NULL)
	{
		_is_xy = _is_xz = _is_yz = false;
		M3DVector3f width;
		m3dSubtractVectors3(width, right_up, left_up);
		M3DVector3f height;
		m3dSubtractVectors3(height, right_up, right_down);
		_width = _height = 0;
		for (int i = 0; i < 3; i++)
		{
			width[i] = fabs(width[i]);
			_width += width[i];
			height[i] = fabs(height[i]);
			_height += height[i];
		}

		if (width[2] < 1e-3 && height[2] < 1e-3)
		{
			_is_xy = true;
		}

		if (width[1] < 1e-3 && height[1] < 1e-3)
		{
			_is_xz = true;
		}

		if (width[0] < 1e-3 && height[0] < 1e-3)
		{
			_is_yz = true;
		}

		m3dCopyVector3(_color, color);
		m3dCopyVector3(_left_down, left_down);
		_kd = 1.0;
		_ka = 0.3;
		_ks = 0.2;

		_ks2 = _ks;
		_kt = 0.0;

		_ws = 0.0;
		_wt = 0.0;	//No Refraction
	}

public:
	~Wall(void)
	{
		if (_texture != NULL)
		{
			delete[] _texture->data;
			delete _texture;
		}
	}

public:
	Intersect_Cond	intersection_check(const M3DVector3f start, const M3DVector3f dir, float& distance, M3DVector3f intersection_p);
	void	shade(M3DVector3f view, M3DVector3f place_ment, const Light& sp_light, M3DVector3f am_light, M3DVector3f color, bool shadow);
	void	get_reflect_direct(const M3DVector3f direct, const M3DVector3f place_ment, M3DVector3f reflect_direct);
	void	get_properties(float& k_s, float& k_t, float& w_s, float& w_t) const { k_s = _ks2; k_t = _kt; w_s = _ws; w_t = _wt; }
	void	set_properties(float k_s, float  k_t, float  w_s, float  w_t) { _ks2 = _ks = k_s; _kt = k_t; _ws = w_s; _wt = w_t; }
public:
	void load_texture(std::string file_name);
private:
	inline void	get_color(M3DVector3f pos, M3DVector3f color) { if (_texture == NULL) m3dCopyVector3(color, _color); else texture_color(pos, color); }
	void	texture_color(M3DVector3f pos, M3DVector3f color);
	void	sphere_space(float x, float y, M3DVector3f color);
private:
	Triangle	_tr1;
	Triangle	_tr2;
	M3DVector3f _color;
	float		_kd;
	float		_ks;
	float		_ka;

	float		_ws;
	float		_wt;
	float		_kt;
	float		_ks2;

private:
	Image* _texture;
	M3DVector3f	_left_down;
	float		_width;
	float		_height;
	bool		_is_xy;
	bool		_is_yz;
	bool		_is_xz;
};
