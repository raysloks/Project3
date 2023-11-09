#include "Polygon.h"

#include "Model.h"

std::shared_ptr<Model> Polygon::makeSweep(intmax_t steps) const
{
	auto model = std::make_shared<Model>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, model, steps]()
	{
		shared_this->loaded.wait(false);

		model->vertices.resize(shared_this->vertices.size() * steps);
		model->triangles.resize(shared_this->vertices.size() * steps * 2);

		intmax_t vertex_count = 0;
		intmax_t triangle_count = 0;

		for (size_t i = 0; i < shared_this->vertices.size(); ++i)
		{
			for (size_t j = 0; j < steps; ++j)
			{
				
			}
		}

		model->vertices.resize(vertex_count);
		model->triangles.resize(triangle_count);

		model->loaded = true;
		model->loaded.notify_all();
	};

	if (loaded)
		func();
	else
		thread_pool.push(func);

	return model;
}
