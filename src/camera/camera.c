#include "camera.h"
#include "math/utils.h"
#include <math.h>

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

PCamera pcamera_new(v3 pos, v3 dir, f32 sensitivity, PCamera_Info info) {
	dir = v3_normalize(dir);
	v3 up = { 0.0f, 1.0f, 0.0f };
	v3 right = v3_cross(dir, up);
	return (PCamera) {
		.pos = pos,
		.dir = dir,
		.up = up,
		.right = right,
		.pitch = 0.0f,
		.yaw = -90.0f,
		.mp = (v2) { 0, 0 },
		.sensitivity = sensitivity,
		.first = true,
		.mouse_enable = false,
		.info = info
	};
}

void pcamera_change_pos(PCamera* cam, v3 dp) {
	cam->pos = v3_add(cam->pos, dp);
}

void pcamera_handle_mouse(PCamera* cam, Window window) {
	if (!cam->mouse_enable) {
		glfwSetInputMode(
			window.glfw_window,
			GLFW_CURSOR,
			GLFW_CURSOR_NORMAL
		);
		return;
	}

	glfwSetInputMode(
		window.glfw_window,
		GLFW_CURSOR,
		GLFW_CURSOR_DISABLED
	);

	if (cam->first) {
		event_set_mouse_pos(
			window, (v2) {
				window.width / 2.0f,
				window.height / 2.0f
			}
		);
		cam->mp = event_mouse_pos(window);
		cam->first = false;
	}

	v2 p = event_mouse_pos(window);
	v2 dp = v2_sub(cam->mp, p);
	v2 sp = v2_mul_scalar(dp, cam->sensitivity);

	cam->yaw -= sp.x;
	cam->pitch += sp.y;

	if (cam->pitch > 89.0f) {
		cam->pitch = 89.0f;
	}
	else if (cam->pitch < -89.0f) {
		cam->pitch = -89.0f;
	}

	v3 front;
	front.x = cos(to_radians(cam->yaw)) * cos(to_radians(cam->pitch));
	front.y = sin(to_radians(cam->pitch));
	front.z = sin(to_radians(cam->yaw)) * cos(to_radians(cam->pitch));

	cam->dir = v3_normalize(front);
	cam->right = v3_normalize(
		v3_cross(cam->dir, (v3) { 0, 1, 0 })
	);
	cam->up = v3_normalize(
		v3_cross(cam->right, cam->dir)
	);

	cam->mp = p;
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
