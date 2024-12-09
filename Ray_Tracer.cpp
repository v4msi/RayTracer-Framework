#include "Ray_Tracer.h"

Ray_Tracer::Ray_Tracer(void)
{
	float	dim = 512;
	_dim[0] = dim;
	_dim[1] = dim;
	_dim[2] = dim;
	_scene.set_dim(_dim);
	_scene.assemble();
	_view_plane.set_origin(0, 0, _dim[2]);
	_view_plane.set_u(1.0, 0.0, 0.0);
	_view_plane.set_v(0.0, 1.0, 0.0);
	_view_plane.set_eye(_dim[0] * 1 / 2.0, _dim[1] / 2.0, _dim[2] + 1000);

	_min_weight = 0.01;
	_max_depth = 0;
}

Ray_Tracer::~Ray_Tracer(void)
{	}


void	Ray_Tracer::run(Image& image)
{
	image.ncolorChannels = 3;
	image.nx = _dim[0];
	image.ny = _dim[1];

	image.n = image.nx * image.ny * image.ncolorChannels;
	image.data = new unsigned char[image.n];
	image.fdata = new float[image.n];

	M3DVector3f	ray;
	M3DVector3f	color;
	M3DVector3f	pij;
	float	weight = 1.0;
	float dx = 1;
	float dy = 1;
	int percent = 0;
	printf("Raytracing is in process..\n");
	printf("Current level:  %2d%%", percent);
	for (int j = 0; j < image.ny; j++)
	{
		for (int i = 0; i < image.nx; i++)
		{
			_view_plane.get_pij(pij, i * dx, j * dy);
			_view_plane.get_per_ray(ray, pij);

			ray_tracing(pij, ray, weight, color, 0, NULL);
			unsigned int index = j * image.nx + i;
			for (int k = 0; k < 3; k++)
			{
				image.fdata[index * 3 + k] = color[k];
			}
		}

		percent = (int)((j + 1) * 100.0 / image.ny);
		printf("\b\b\b\b%3d%%", percent);
	}

	percent = 100;
	printf("\b\b\b\b%3d%%", percent);
	printf("\nRay tracing Completed!!! \n");

	//Normalizing Image
	double max_v = 0;
	double min_v = 1e+6;
	for (int m = 0; m < image.n; m++)
	{
		if (image.fdata[m] > max_v)
		{
			max_v = image.fdata[m];
		}
		if (image.fdata[m] < min_v)
		{
			min_v = image.fdata[m];
		}
	}

	if (max_v > 1e-5)
	{
		for (int m = 0; m < image.n; m++)
		{
			image.data[m] = (unsigned char)((image.fdata[m]) * 255.0/max_v);
			if (image.data[m] > 255)
			{
				image.data[m] = 255;
			}
			
		}
	}else
	{
		printf("Error:happens in Normalizing the image into (0,255)....\n");
	}
}


void Ray_Tracer::ray_tracing(M3DVector3f start, M3DVector3f direct, float weight, M3DVector3f color, unsigned int depth, Basic_Primitive* prim_in)
{
	Basic_Primitive* prim = NULL;
	M3DVector3f place_ment;
	if (weight < _min_weight)
	{
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 0.0;
		return;
	}
	Intersect_Cond type = _scene.intersection_check(start, direct, &prim, place_ment);
	if (type != _k_miss)
	{
		M3DVector3f	am_light;
		M3DVector3f	reflect_direct, refract_direct;
		M3DVector3f local_color, reflect_color, refract_color;
		float k_s, k_t, ws, wt;
		prim->get_properties(k_s, k_t, ws, wt);
		_scene.get_amb_light(am_light);
		prim->shade(direct, place_ment, _scene.get_sp_light(), am_light, local_color, shadow_verify(place_ment));
		if (k_s > 0)
		{
			prim->get_reflect_direct(direct, place_ment, reflect_direct);
			ray_tracing(place_ment, reflect_direct, weight * ws, reflect_color, depth + 1, prim_in);
		}
		if (k_t > 0)
		{
			if (!prim->get_refract_direct(direct, place_ment, refract_direct, prim->get_delta(), type == _k_inside))
			{
				prim->get_reflect_direct(direct, place_ment, refract_direct);
			}

			ray_tracing(place_ment, refract_direct, weight * wt, refract_color, depth + 1, prim_in);
		}
		for (int k = 0; k < 3; k++)
		{
			float kk = 1 - (k_s + k_t);
			color[k] = local_color[k] * kk;
			color[k] += reflect_color[k] * k_s;
			color[k] += refract_color[k] * k_t;
		}
	}
	else
	{
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 0.0;
	}

}

bool Ray_Tracer::shadow_verify(M3DVector3f place_ment)
{
	const Light& sp_light = _scene.get_sp_light();
	M3DVector3f	intersect_to_light, sp_light_pos;
	sp_light.get_light_pos(sp_light_pos);
	m3dSubtractVectors3(intersect_to_light, place_ment, sp_light_pos);
	float distance = m3dDotProduct(intersect_to_light, intersect_to_light);
	m3dNormalizeVector(intersect_to_light);

	Basic_Primitive* prim = NULL;
	M3DVector3f intersect_p_n;
	if (_scene.intersection_check(sp_light_pos, intersect_to_light, &prim, intersect_p_n) == _k_hit)
	{
		M3DVector3f incr;
		m3dCopyVector3(incr, intersect_to_light);
		m3dScaleVector3(incr, 0.05);
		m3dAddVectors3(intersect_p_n, intersect_p_n, incr);

		M3DVector3f intersect;
		m3dSubtractVectors3(intersect, sp_light_pos, intersect_p_n);
		float new_distance = m3dDotProduct(intersect, intersect);
		if (new_distance < distance)
		{
			return true;
		}
	}
	return false;
}

