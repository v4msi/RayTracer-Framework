#include "Wall.h"
#include <math.h>
#include "../Imageio/Imageio.h"

Intersect_Cond Wall::intersection_check(const M3DVector3f start, const M3DVector3f dir, float& distance, M3DVector3f intersection_p)
{
	if (_tr1.intersection_check(start, dir, distance, intersection_p) == _k_hit)
	{
		return _k_hit;
	}
	else
	{
		return _tr2.intersection_check(start, dir, distance, intersection_p);
	}
}

void	Wall::shade(M3DVector3f view, M3DVector3f place_ment, const Light& sp_light, M3DVector3f am_light, M3DVector3f color, bool shadow)
{
	M3DVector3f normal;
	_tr1.normal(normal);
	m3dNormalizeVector(normal);
	M3DVector3f direction_lit, postn_lit, light_color;
	sp_light.get_light(postn_lit, light_color);
	m3dSubtractVectors3(direction_lit, postn_lit, place_ment);
	m3dNormalizeVector(direction_lit);
	float dot = m3dDotProduct(direction_lit, normal);
	float cal_culat = dot;
	if (cal_culat < 0) cal_culat = 0;
	if (shadow) cal_culat = 0;
	float sp_coeff;
	M3DVector3f r;
	M3DVector3f  tmp;
	m3dCopyVector3(tmp, normal);
	m3dScaleVector3(tmp, (2 * dot));
	m3dSubtractVectors3(r, direction_lit, tmp);
	sp_coeff = m3dDotProduct(view, r);
	if (sp_coeff < 0) sp_coeff = 0;
	if (shadow) sp_coeff = 0;
	sp_coeff = pow(sp_coeff, 11);
	for (int i = 0; i < 3; i++)
	{
		color[i] = _kd * cal_culat + _ks * sp_coeff * light_color[i] + _ka * am_light[i];
		color[i] *= _color[i];
	}
	M3DVector3f t_color;
	get_color(place_ment, t_color);
	for (int i = 0; i < 3; i++)
	{
		color[i] *= t_color[i];
	}

}


void	Wall::get_reflect_direct(const M3DVector3f direct0, const M3DVector3f place_ment, M3DVector3f reflect_direct)
{
	M3DVector3f normal;
	_tr1.normal(normal);

	float dot = m3dDotProduct(direct0, normal);
	m3dScaleVector3(normal, 2 * dot);
	m3dSubtractVectors3(reflect_direct, direct0, normal);
}


void Wall::load_texture(std::string file_name)
{
	_texture = new Image;
	if (_texture == NULL)
	{
		printf("Unable to load  %s. ending\n", file_name.c_str());
		return;
	}
	ReadImage(file_name.c_str(), _texture->nx, _texture->ny, _texture->data, _texture->ncolorChannels);
}

void Wall::texture_color(M3DVector3f pos, M3DVector3f color)
{
	float x = _is_yz ? pos[2] - _left_down[2] : pos[0] - _left_down[0];
	float y = _is_xz ? pos[2] - _left_down[2] : pos[1] - _left_down[1];
	x /= _width;
	y /= _height;
	sphere_space(x, y, color);
}

void	Wall::sphere_space(float x, float y, M3DVector3f color)
{
	int ix = int(x * _texture->nx + _texture->nx) % _texture->nx;
	int iy = int(y * _texture->ny + _texture->ny) % _texture->ny;	
	int i = (ix + iy * _texture->nx) * 3;
	color[0] = _texture->data[i + 0] / 255.0;
	color[1] = _texture->data[i + 1] / 255.0;
	color[2] = _texture->data[i + 2] / 255.0;
}