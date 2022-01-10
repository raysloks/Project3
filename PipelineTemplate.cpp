#include "PipelineTemplate.h"

#include "GraphicsPipeline.h"

PipelineTemplate::PipelineTemplate(ModelRenderSystem * mrs) : mrs(mrs)
{
}

ModelRenderSystem * PipelineTemplate::getModelRenderSystem() const
{
	return mrs;
}

void PipelineTemplate::setRenderPass(const std::shared_ptr<RenderPass>& render_pass)
{
	graphics_pipeline.reset(new GraphicsPipeline(*this, render_pass));
}

std::shared_ptr<GraphicsPipeline> PipelineTemplate::getGraphicsPipeline()
{
	return graphics_pipeline;
}

void PipelineTemplate::setSettings(const Settings& settings)
{
	graphics_pipeline = nullptr;
	this->settings = settings;
}

PipelineTemplate::Settings PipelineTemplate::getSettings() const
{
	return settings;
}
