#include "renderer.h"

Result_Renderer renderer_new(ECS* ecs, v2 surf_size, v2 win_size) {

	// Setting up IMR
	Result_IMR r_imr = imr_new();
	if (r_imr.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_imr));
	}

	// Setting up cameras
	PCamera ren_cam = pcamera_new(
		(v3) { 0, 0, 1 },
		(v3) { 0, 0, -1 },
		0.1f,
		(PCamera_Info) {
			.aspect_ratio = (f32) surf_size.x / surf_size.y,
			.fov = 45.0f,
			.near = 0.0f,
			.far = 1000.0f
		}
	);

	PCamera final_cam = pcamera_new(
		(v3) { 0, 0, 1 },
		(v3) { 0, 0, -1 },
		0.1f,
		(PCamera_Info) {
			.aspect_ratio = (f32) win_size.x / win_size.y,
			.fov = 45.0f,
			.near = 0.0f,
			.far = 1000.0f
		}
	);

	// Setting up frame buffers
	Result_FBO r_light_fbo = fbo_new(surf_size.x, surf_size.y);
	if (r_light_fbo.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_light_fbo));
	}

	Result_FBO r_color_fbo = fbo_new(surf_size.x, surf_size.y);
	if (r_color_fbo.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_color_fbo));
	}

	Result_FBO r_mix_fbo = fbo_new(surf_size.x, surf_size.y);
	if (r_mix_fbo.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_mix_fbo));
	}

	return OK(Renderer, (Renderer) {
		.imr = unwrap(r_imr),
		.ecs = ecs,
		.ren_cam = ren_cam,
		.final_cam = final_cam,
		.surf_size = surf_size,
		.win_size = win_size,
		.light_fbo = unwrap(r_light_fbo),
		.color_fbo = unwrap(r_color_fbo),
		.mix_fbo = unwrap(r_mix_fbo),
	});
}

void renderer_delete(Renderer* ren) {
	imr_delete(&ren->imr);
	fbo_delete(&ren->light_fbo);
	fbo_delete(&ren->color_fbo);
	fbo_delete(&ren->mix_fbo);
}

void renderer_update(Renderer* ren, v4 color) {
	// Color pass
	{
		glViewport(0, 0, ren->surf_size.x, ren->surf_size.y);
		fbo_bind(&ren->color_fbo);

		imr_clear(color);
		imr_begin(&ren->imr);
	
		m4 mvp = pcamera_calc_mvp(&ren->ren_cam);
		imr_update_mvp(&ren->imr, mvp);
	
		CompRecord* cr = comp_table_get_record(ren->ecs->table, RenderComponent);
		if (cr != NULL) {
			for (int i = 0; i < cr->entry_cnt; i++) {
				Entity ent = cr->entries_ent[i];
	
				CompEntry* entry = comp_record_get_entry(cr, ent);
				RenderComponent* rc = entry->data;
				TransformComponent* tc = entity_get_component(ren->ecs, ent, TransformComponent);
	
				imr_push_quad(
					&ren->imr,
					tc->pos,
					tc->size,
					rotate_z(0),
					rc->color
				);
			}
		}
	
		imr_end(&ren->imr);

		fbo_unbind();
	}

	// Final pass
	{
		glViewport(0, 0, ren->win_size.x, ren->win_size.y);
		imr_clear((v4) {0, 0, 0, 1});
		imr_begin(&ren->imr);
	
		m4 mvp = pcamera_calc_mvp(&ren->final_cam);
		imr_update_mvp(&ren->imr, mvp);

		Texture texture_to_render = ren->color_fbo.color_texture;
		texture_bind(texture_to_render);
	
		imr_push_quad_tex(
			&ren->imr,
			(v3) {-1, -1, 0},
			(v2) {2, 2},
			(Rect) {0, 0, 1, 1},
			texture_to_render.id,
			rotate_z(0),
			(v4) {1, 1, 1, 1}
		);
	
		imr_end(&ren->imr);
	}
}
