#include "RenderContext.h"

#include "RenderingModel.h"
#include "ModelRenderSystem.h"

RenderContext::RenderContext(ModelRenderSystem * mrs, size_t image_index, const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<GraphicsPipeline>& graphics_pipeline) : mrs(mrs), image_index(image_index), render_pass(render_pass), graphics_pipeline(graphics_pipeline)
{
    dynamic_state.scissor = { 0, 0, mrs->getWidth(), mrs->getHeight() };
}

ModelRenderSystem * RenderContext::getModelRenderSystem() const
{
    return mrs;
}

size_t RenderContext::getImageIndex() const
{
    return image_index;
}

std::shared_ptr<RenderPass> RenderContext::getRenderPass() const
{
    return render_pass;
}

std::shared_ptr<GraphicsPipeline> RenderContext::getGraphicsPipeline() const
{
    return graphics_pipeline;
}

void RenderContext::addRenderingModel(const std::shared_ptr<RenderingModel>& rendering_model)
{
    rendering_models.push_back(rendering_model);
}

void RenderContext::collectCommandBuffers()
{
    for (auto& rendering_model : rendering_models)
        command_buffers.push_back(rendering_model->getCommandBuffer(image_index));
}

void RenderContext::executeCommands(VkCommandBuffer command_buffer) const
{
    vkCmdExecuteCommands(command_buffer, (uint32_t)command_buffers.size(), command_buffers.data());
}

void RenderContext::pushDynamicState()
{
    dynamic_states.push_back(dynamic_state);
}

void RenderContext::popDynamicState()
{
    dynamic_state = dynamic_states.back();
    dynamic_states.pop_back();
}
