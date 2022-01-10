#include "RenderPassTemplate.h"

#include "RenderPass.h"

RenderPassTemplate::RenderPassTemplate(ModelRenderSystem * mrs) : mrs(mrs)
{
}

ModelRenderSystem * RenderPassTemplate::getModelRenderSystem() const
{
	return mrs;
}

std::shared_ptr<RenderPass> RenderPassTemplate::getRenderPass() const
{
	return render_pass;
}

void RenderPassTemplate::setSettings(const Settings& settings)
{
	this->settings = settings;
	render_pass.reset(new RenderPass(*this));
}

RenderPassTemplate::Settings RenderPassTemplate::getSettings() const
{
	return settings;
}
