#include "camera.h"

// Orthographic camera

OCamera ocamera_new(v2 pos, f32 zoom, OCamera_Boundary boundary) {
	return (OCamera) {
		.pos = pos,
		.zoom = zoom,
		.mvp = m4_zero(),
		.org_b = boundary,
		.boundary = (OCamera_Boundary) {
			boundary.left / zoom,
			boundary.right / zoom,
			boundary.top / zoom,
			boundary.bottom / zoom,
			boundary.near,
			boundary.far
		}
	};
}

void ocamera_change_zoom(OCamera* cam, f32 dz) {
	cam->zoom += dz;
	cam->boundary = (OCamera_Boundary) {
		cam->org_b.left   / cam->zoom,
		cam->org_b.right  / cam->zoom,
		cam->org_b.top    / cam->zoom,
		cam->org_b.bottom / cam->zoom,
		cam->org_b.near,
		cam->org_b.far
	};
}

void ocamera_change_pos(OCamera* cam, v2 dp) {
	cam->pos = v2_add(cam->pos, dp);
}

m4 ocamera_calc_mvp(OCamera* cam) {
	m4 proj = ortho_projection(
		cam->boundary.left,
		cam->boundary.right,
		cam->boundary.top,
		cam->boundary.bottom,
		cam->boundary.near,
		cam->boundary.far
	);
	m4 transform = m4_translate(
		m4_identity(),
		(v3) {
			cam->pos.x,
			cam->pos.y,
			0
		}
	);
	m4 transpose = m4_transpose(transform);
	m4 view_mat = m4_inverse(transpose);
	cam->mvp = m4_transpose(m4_mul(proj, view_mat));
	return cam->mvp;
}

// Perspective camera

PCamera pcamera_new(v3 pos, v3 dir, PCamera_Info info) {
	dir = v3_normalize(dir);
	v3 up = { 0.0f, 1.0f, 0.0f };
	v3 right = v3_cross(dir, up);
	return (PCamera) {
		.pos = pos,
		.dir = dir,
		.up = up,
		.right = right,
		.info = info
	};
}

void pcamera_change_pos(PCamera* cam, v3 dp) {
	cam->pos = v3_add(cam->pos, dp);
}

m4 pcamera_calc_mvp(PCamera* cam) {
	m4 proj = persp_projection(
		cam->info.aspect_ratio,
		cam->info.fov,
		cam->info.near,
		cam->info.far
	);

	m4 camera_mat = {
		.m = {
			{ cam->right.x, cam->right.y, cam->right.z, 0.0f },
			{ cam->up.x   , cam->up.y   , cam->up.z   , 0.0f },
			{ cam->dir.x  , cam->dir.y  , cam->dir.z  , 0.0f },
			{         0.0f,         0.0f,         0.0f, 1.0f }
		}
	};

	m4 camera_trans = m4_translate(
		m4_identity(),
		(v3) { -cam->pos.x, -cam->pos.y, -cam->pos.z }
	);

	cam->look_at = m4_mul(camera_mat, camera_trans);
	cam->mvp = m4_mul(proj, cam->look_at);
	return cam->mvp;
}
