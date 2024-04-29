#include "camera.h"

OCamera ocamera_new(v2 pos, OCamera_Boundary boundary) {
	return (OCamera) {
		.pos = pos,
		.mvp = m4_zero(),
		.boundary = boundary
	};
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
