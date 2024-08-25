#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

typedef struct {
	v3 pos;
	v2 size;
} TransformComponent;

typedef struct {
	v4 color;
} RenderComponent;

#endif // __COMPONENTS_H__
