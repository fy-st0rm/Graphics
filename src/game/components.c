#include "components.h"
#include <math.h>

AnimationComponent make_animation_component(void* entries, i32 starting_state) {
	return (AnimationComponent) {
		.entries = entries,
		.curr_state = starting_state,
		.start_time = glfwGetTime()
	};
}

void ac_switch_frame(AnimationComponent* ac, i32 id) {
}

Rect ac_get_frame(AnimationComponent* ac) {
	dyn_array_for_each(ac->entries, AnimationEntry entry, {
		if (entry.id != ac->curr_state)
			continue;

		f64 dt  = (glfwGetTime() - ac->start_time) * 1000;
		if (dt >= entry.duration) {
			ac->start_time = glfwGetTime();
		}
		dt  = (glfwGetTime() - ac->start_time) * 1000;

		f64 fpt = entry.frames->len / entry.duration;
		i32 idx = fpt * dt;

		return dyn_array_get(entry.frames, idx);
	});
}
