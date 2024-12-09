#include "Vamsi.h"
#include <math.h>
#include "../Imageio/Imageio.h"
#include "../common/math3d.h"

Intersect_Cond Object::intersection_check(const M3DVector3f start, const M3DVector3f dir, float& dist, M3DVector3f intersection_p)
{
	Intersect_Cond cond = _k_miss;
	_tr = NULL;
	dist = 1e+6;
	float length;
	M3DVector3f p;

	for (Triangle_List::iterator it = _triangle_list.begin(); it != _triangle_list.end(); it++)
	{
		Triangle* tr = *it;

		if (tr->intersection_check(start, dir, length, p) != _k_hit)
			continue;

		if (length < dist)
		{
			cond = _k_hit;
			_tr = tr;
			m3dCopyVector3(intersection_p, p);
			dist = length;
		}
	}

	return cond;
}


void Object::shade(M3DVector3f view, M3DVector3f place_ment, const Light& sp_light, M3DVector3f am_light, M3DVector3f color, bool shadow)
{
	M3DVector3f no_filtr;
	_tr->normal(no_filtr);
	m3dScaleVector3(no_filtr, -1);

	M3DVector3f ltdr, ltpos, ltcolor;
	sp_light.get_light(ltpos, ltcolor);
	m3dSubtractVectors3(ltdr, ltpos, place_ment);
	m3dNormalizeVector(ltdr);
	float dot = m3dDotProduct(ltdr, no_filtr);
	float diff = dot;
	if (diff < 0) diff = 0;
	if (shadow) diff = 0;
	float sp_coeff;
	M3DVector3f r;
	M3DVector3f  tmp;
	m3dCopyVector3(tmp, no_filtr);
	m3dScaleVector3(tmp, (2 * dot));
	m3dSubtractVectors3(r, ltdr, tmp);
	sp_coeff = m3dDotProduct(view, r);
	if (sp_coeff < 0) sp_coeff = 0;
	if (shadow) sp_coeff = 0;
	sp_coeff = pow(sp_coeff, 11);
	//Generate color with Phong model
	for (int i = 0; i < 3; i++)
	{
		color[i] = _kd * diff + _ks * sp_coeff * ltcolor[i] + _ka * am_light[i];
		color[i] *= _color[i];
	}
}


void Object::get_reflect_direct(const M3DVector3f direct0, const M3DVector3f place_ment, M3DVector3f reflect_direct)
{
	//implementation for general and relection
	M3DVector3f no_filtr;
	_tr->normal(no_filtr);
	m3dScaleVector3(no_filtr, -1);


	float dot = m3dDotProduct(direct0, no_filtr);
	m3dScaleVector3(no_filtr, 2 * dot);
	m3dSubtractVectors3(reflect_direct, direct0, no_filtr);
}

bool Object::get_refract_direct(const M3DVector3f direct, const M3DVector3f place_ment, M3DVector3f refract_direct, float delta, bool is_in)
{
	//Normal vector and refract 
	M3DVector3f no_filtr;
	_tr->normal(no_filtr);
	m3dScaleVector3(no_filtr, -1);

	float rflct = is_in ? 1 / delta : delta;
	m3dScaleVector3(no_filtr, is_in ? -1.0 : 1.0);
	float dot = m3dDotProduct(direct, no_filtr) * rflct;
	float ca_ch = rflct * rflct - dot * dot;
	if (ca_ch < 1)
	{
		m3dCopyVector3(refract_direct, direct);
		m3dScaleVector3(refract_direct, rflct);
		m3dScaleVector3(no_filtr, dot + sqrtf(1 - ca_ch));
		m3dSubtractVectors3(refract_direct, refract_direct, no_filtr);
		return true;
	}

	return false;
}