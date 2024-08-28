#include "renderer.h"

Result_Renderer renderer_new(ECS* ecs, v2 surf_size, v2 win_size) {

	// Setting up IMR
	Result_IMR r_imr = imr_new();
	if (r_imr.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_imr));
	}

	// Setting up cameras
	OCamera final_cam = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, win_size.x, win_size.y, 0, -1, 1000 }
	);

	// Loading shaders
	Result_Shader r_color_shader = shader_new(color_vertex_src, color_fragment_src);
	if (r_color_shader.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_color_shader));
	}

	Result_Shader r_light_shader = shader_new(light_vertex_src, light_fragment_src);
	if (r_light_shader.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_light_shader));
	}

	Result_Shader r_mix_shader = shader_new(light_vertex_src, mix_fragment_src);
	if (r_mix_shader.status == ERROR) {
		return ERR(Renderer, unwrap_err(r_mix_shader));
	}

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
		.final_cam = final_cam,
		.surf_size = surf_size,
		.win_size = win_size,
		.color_shader = unwrap(r_color_shader),
		.light_shader = unwrap(r_light_shader),
		.mix_shader = unwrap(r_mix_shader),
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

void renderer_update(Renderer* ren, OCamera* camera, v4 color) {
	// Color pass
	renderer_color_pass(ren, camera, color);

	// Light pass
	renderer_light_pass(ren, camera, color);

	// Mix pass
	renderer_mix_pass(ren, camera, color);

	// Final pass
	{
		imr_switch_shader_to_default(&ren->imr);

		glViewport(0, 0, ren->win_size.x, ren->win_size.y);
		imr_clear((v4) {0, 0, 0, 1});
		imr_begin(&ren->imr);
	
		m4 mvp = ocamera_calc_mvp(&ren->final_cam);
		imr_update_mvp(&ren->imr, mvp);

		Texture texture_to_render = ren->mix_fbo.color_texture;
		texture_bind(texture_to_render);
	
		imr_push_quad_tex(
			&ren->imr,
			(v3) {0, 0, 0},
			ren->win_size,
			(Rect) {0, 0, 1, 1},
			texture_to_render.id,
			rotate_z(0),
			(v4) {1, 1, 1, 1}
		);
	
		imr_end(&ren->imr);
	}
}

void renderer_push_light_uniforms(Renderer* ren) {
	CompRecord* cr = comp_table_get_record(ren->ecs->table, LightComponent);
	if (cr == NULL) return;

	int loc;

	loc = GLCall(glGetUniformLocation(ren->color_shader, "dim"));
	assert(loc != -1, "Cannot find uniform: dim\n");
	GLCall(glUniform2f(loc, ren->surf_size.x, ren->surf_size.y));

	loc = GLCall(glGetUniformLocation(ren->color_shader, "light_cnt"));
	assert(loc != -1, "Cannot find uniform: light_cnt\n");
	GLCall(glUniform1i(loc, cr->entry_cnt));

	for (int i = 0; i < cr->entry_cnt; i++) {
		Entity ent = cr->entries_ent[i];
		CompEntry* entry = comp_record_get_entry(cr, ent);
		LightComponent* light = entry->data;

		char* uni_name = "light[%d].%s";
		char buff[100];

		sprintf(buff, uni_name, i, "pos");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: %s\n", buff);
		GLCall(glUniform2f(loc, light->pos.x, light->pos.y));

		sprintf(buff, uni_name, i, "radius");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: %s\n", buff);
		GLCall(glUniform1f(loc, light->radius));

		sprintf(buff, uni_name, i, "intensity");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: light->intensity\n");
		GLCall(glUniform1f(loc, light->intensity));

		sprintf(buff, uni_name, i, "dir");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: light->dir\n");
		GLCall(glUniform1f(loc, light->dir));

		sprintf(buff, uni_name, i, "fov");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: light->fov\n");
		GLCall(glUniform1f(loc, light->fov));

		sprintf(buff, uni_name, i, "color");
		loc = GLCall(glGetUniformLocation(ren->color_shader, buff));
		assert(loc != -1, "Cannot find uniform: light->color\n");
		GLCall(glUniform4f(loc, light->color.r, light->color.g, light->color.b, light->color.a));
	}
}

void renderer_color_pass(Renderer* ren, OCamera* camera, v4 color) {
	imr_switch_shader(&ren->imr, ren->color_shader);
	imr_reapply_samplers(&ren->imr);

	glViewport(0, 0, ren->surf_size.x, ren->surf_size.y);
	fbo_bind(&ren->color_fbo);

	imr_clear(color);
	imr_begin(&ren->imr);

	m4 mvp = ocamera_calc_mvp(camera);
	imr_update_mvp(&ren->imr, mvp);

	// Handling light
	renderer_push_light_uniforms(ren);

	// Handling render component
	{
		CompRecord* cr = comp_table_get_record(ren->ecs->table, RenderComponent);
		if (cr != NULL) {
			for (int i = 0; i < cr->entry_cnt; i++) {
				Entity ent = cr->entries_ent[i];
	
				CompEntry* entry = comp_record_get_entry(cr, ent);
				RenderComponent* rc = entry->data;
				TransformComponent* tc = entity_get_component(ren->ecs, ent, TransformComponent);
	
				imr_push_quad_tex(
					&ren->imr,
					tc->pos,
					tc->size,
					rc->tex_coord,
					rc->texture.id,
					rotate_z(0),
					rc->color
				);
			}
		}
	}

	imr_end(&ren->imr);
	fbo_unbind();
}

void renderer_light_pass(Renderer* ren, OCamera* camera, v4 color) {
	imr_switch_shader(&ren->imr, ren->light_shader);

	glViewport(0, 0, ren->surf_size.x, ren->surf_size.y);
	fbo_bind(&ren->light_fbo);

	imr_clear(color);
	imr_begin(&ren->imr);
	
	m4 mvp = ocamera_calc_mvp(camera);
	imr_update_mvp(&ren->imr, mvp);

	// Handling light
	renderer_push_light_uniforms(ren);

	imr_push_quad(
		&ren->imr,
		(v3) { -1, -1, 0 },
		(v2) { 2, 2 },
		rotate_z(0),
		(v4) { 1, 1, 1, 1 }
	);

	imr_end(&ren->imr);
	fbo_unbind();
}

void renderer_mix_pass(Renderer* ren, OCamera* camera, v4 color) {
	imr_switch_shader(&ren->imr, ren->mix_shader);

	glViewport(0, 0, ren->surf_size.x, ren->surf_size.y);
	fbo_bind(&ren->mix_fbo);

	imr_clear(color);
	imr_begin(&ren->imr);

	texture_bind(ren->color_fbo.color_texture);
	texture_bind(ren->light_fbo.color_texture);

	int loc;

	loc = GLCall(glGetUniformLocation(ren->mix_shader, "color_texture"));
	assert(loc != -1, "Cannot find uniform: color_texture\n");
	GLCall(glUniform1i(loc, ren->color_fbo.color_texture.id));

	loc = GLCall(glGetUniformLocation(ren->mix_shader, "light_texture"));
	assert(loc != -1, "Cannot find uniform: light_texture\n");
	GLCall(glUniform1i(loc, ren->light_fbo.color_texture.id));

	imr_push_quad(
		&ren->imr,
		(v3) { -1, -1, 0 },
		(v2) { 2, 2},
		rotate_z(0),
		(v4) { 1, 1, 1, 1 }
	);

	imr_end(&ren->imr);
	fbo_unbind();
}
