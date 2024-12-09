
#include "Scene.h"
#include "../RayTracer/Vamsi.h"

Scene::Scene()
{
}

Scene::~Scene(void)
{
	Basic_Primitive* prim = NULL;
	for (Prim_List::iterator it = _prim_list.begin(); it != _prim_list.end();)
	{
		prim = *it;
		_prim_list.erase(it++);
		delete prim;
	}
}

void	Scene::assemble()
{

	m3dLoadVector3(_amb_light, 0.9, 0.9, 0.8);
	
	M3DVector3f col_lgt = { 1.0, 0.8, 0.8 };
	
	M3DVector3f pst_lgt = { _dim[0] * 0.6, _dim[1] * 0.7, _dim[2] * 0.6 };
	_sp_light.set_light(pst_lgt, col_lgt);


	M3DVector3f	x0y0z0 = { 0, 0, 0 };
	M3DVector3f	x1y0z0 = { _dim[0], 0, 0 };
	M3DVector3f	x0y1z0 = { 0,_dim[1], 0 };
	M3DVector3f	x1y1z0 = { _dim[0], _dim[1], 0 };
	M3DVector3f	x0y0z1 = { 0,0,_dim[2] };
	M3DVector3f	x1y0z1 = { _dim[0],0,_dim[2] };
	M3DVector3f	x0y1z1 = { 0,_dim[1],_dim[2] };
	M3DVector3f	x1y1z1 = { _dim[0],_dim[1],_dim[2] };

	M3DVector3f wall_color_left = { 1.0,0.9,1.0 };
	//Walls
	Wall* left_wall = new Wall(x0y1z0, x0y1z1, x0y0z1, x0y0z0, wall_color_left);
	left_wall->load_texture("images\\rock_wall.ppm");
	_prim_list.push_back(left_wall); // Left wall

	M3DVector3f wall_color_right = { 1.0,1.0,1.0 };
	Wall* right_wall = new Wall(x1y1z1, x1y1z0, x1y0z0, x1y0z1, wall_color_right);
	right_wall->load_texture("images\\rock_wall.ppm");
	_prim_list.push_back(right_wall);	// Right wall

	M3DVector3f wall_color = { 1.0,1.0,1.0 };
	Wall* top_wall = new Wall(x1y1z1, x0y1z1, x0y1z0, x1y1z0, wall_color);
	top_wall->load_texture("images\\rock_wall.ppm");
	_prim_list.push_back(top_wall);	// Top wall

	Wall* bottom_wall = new Wall(x1y0z1, x1y0z0, x0y0z0, x0y0z1, wall_color);
	bottom_wall->set_properties(0.7, 0, 0.7, 0);
	bottom_wall->load_texture("images\\rock_wall.ppm");
	_prim_list.push_back(bottom_wall);	// Bottom wall

	M3DVector3f back_wall_color = { 1.0,1.0,1.0 };
	Wall* back_wall = new Wall(x1y1z0, x0y1z0, x0y0z0, x1y0z0, back_wall_color);
	back_wall->load_texture("images\\nature.ppm");
	_prim_list.push_back(back_wall);	// Back wall

//End of walls

//Sphere
	float	rad = _dim[2] / 6.0;
	M3DVector3f sphere_center = { rad + 20, rad, 150 };
	M3DVector3f sphere_color = { 1.0, 1.0, 1.0 };
	Sphere* sp = new Sphere(sphere_center, rad, sphere_color);
	sp->load_texture("images\\earth.ppm");
	sp->set_properties(0.0, 0.0, 0.0, 0.0);
	_prim_list.push_back(sp);
	//End of Sphere

	//Add 3D object
	float table_size = _dim[2] * 0.3;
	M3DVector3f table_center = { _dim[0] * 0.6, _dim[1] * 0.0, _dim[2] * 0.8 };
	M3DVector3f table_color = { 0.2, 0.8, 0.4 };
	Object* object = new Object("Vamsi.obj", table_center, table_size, table_color);
	object->set_properties(0.0, 0.9, 0.0, 0.9);
	_prim_list.push_back(object);

	//Add light source and ambient lighting
	M3DVector3f lpos = { 350,350,250 };
	M3DVector3f lcolor = { 0.5,0.5,0.5 };
	_sp_light = Light(lpos, lcolor);

	M3DVector3f alight = { 1.0,1.0,1.0 };
	m3dCopyVector3(_amb_light, alight);
}

Intersect_Cond	Scene::intersection_check(const M3DVector3f start0, const M3DVector3f dir, Basic_Primitive** prim_intersect, M3DVector3f closest_point)
{
	M3DVector3f incr, start;
	m3dCopyVector3(incr, dir);
	m3dScaleVector3(incr, 0.01);
	m3dAddVectors3(start, start0, incr);

	Basic_Primitive* prim = NULL;
	float		distance;
	M3DVector3f point;

	float	min_distance = 0x0ffffff;
	*prim_intersect = NULL;

	Intersect_Cond ret = _k_miss;
	Intersect_Cond tmp;
	for (Prim_List::iterator it = _prim_list.begin(); it != _prim_list.end(); it++)
	{
		prim = *it;
		tmp = prim->intersection_check(start, dir, distance, point);
		if (tmp != _k_miss)
		{
			if (distance < min_distance)
			{
				min_distance = distance;
				*prim_intersect = prim;
				m3dCopyVector3(closest_point, point);
				ret = tmp;
			}
		}
	}

	return ret;
}