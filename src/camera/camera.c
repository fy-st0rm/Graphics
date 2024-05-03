#include "camera.h"

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
