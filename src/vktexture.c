#include "grafics2.h"

#define UPDATE_DEBUG_LINE() bp->user_data.line = __LINE__ + 1
#define UPDATE_DEBUG_FILE() bp->user_data.file = __FILE__

void vktexturec(VkTexture* texture, VkBoilerplate* bp, VkCore* core,
				VkmaAllocator* mAllocator, VkbaAllocator* bAllocator)
{
	UPDATE_DEBUG_FILE();
	VkResult result;

	/*
	uint32_t width, height;
	void* pixels = (void*) bmp_load("resources/test2.bmp", &width, &height);
	*/

	TrueTypeFont* ttf = NULL;
	i32 ttf_result = ttf_load(&ttf, "resources/calibri.ttf");
	assert(ttf_result == 1);
	
	u32 width = 128;
	u32 height = width;
	void* pixels = (void*) ttf_create_bitmap(ttf, 'a', width, height);
	ttf_free(&ttf);
	VkFormat format = VK_FORMAT_R8_SRGB;

	VkImageCreateInfo image_info = (VkImageCreateInfo) {
   		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = { width, height, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};

	VkmaAllocationInfo allocInfo = {
		VKMA_ALLOCATION_USAGE_DEVICE,
		0,
		"TEXTURE",
		NULL
	};
	
	result = vkmaCreateImage(mAllocator, &image_info, &texture->image,
							 &allocInfo, &texture->allocation);
	assert(result == VK_SUCCESS);

	vktransitionimglayout(&texture->image, bp, core, format,
						  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkbaVirtualBuffer stagingBuffer;
	VkbaVirtualBufferInfo tmpBufferInfo = { HOST_INDEX, width * height, pixels, 0 };
	result = vkbaCreateVirtualBuffer(bAllocator, &stagingBuffer, &tmpBufferInfo);
	memcpy(stagingBuffer.dst, stagingBuffer.src, stagingBuffer.locale.size);
	
	vkcopybuftoimg(texture, bp, core, &stagingBuffer, width, height);

	vktransitionimglayout(&texture->image, bp, core, format,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageViewCreateInfo view_info = (VkImageViewCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = texture->image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
						VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	};

	UPDATE_DEBUG_LINE();
	result = vkCreateImageView(bp->dev, &view_info, NULL, &texture->view);
	assert(result == VK_SUCCESS);
	logt("VkTexture.view created\n");

	VkPhysicalDeviceProperties phydev_properties;
	vkGetPhysicalDeviceProperties(bp->phydev, &phydev_properties);
	
	VkSamplerCreateInfo sampler_info = (VkSamplerCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.magFilter = VK_FILTER_NEAREST,
		.minFilter = VK_FILTER_NEAREST,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = phydev_properties.limits.maxSamplerAnisotropy,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};

	UPDATE_DEBUG_LINE();
	result =  vkCreateSampler(bp->dev, &sampler_info, NULL, &texture->sampler);
	assert(result == VK_SUCCESS);
	logt("VkTexture.sampler created\n");

	vkbaDestroyVirtualBuffer(bAllocator, &stagingBuffer);
	bmp_free(pixels);

	logt("VkTexture created\n");
}

void vktextured(VkTexture* texture, VkBoilerplate* bp, VkmaAllocator* mAllocator)
{
	vkDestroySampler(bp->dev, texture->sampler, NULL);
	vkDestroyImageView(bp->dev, texture->view, NULL);
	vkmaDestroyImage(mAllocator, &texture->image, &texture->allocation);

	logt("VkTexture destroyed\n");
}

void vktransitionimglayout(VkImage* image, VkBoilerplate* bp, VkCore* core,
						  VkFormat format, VkImageLayout old_layout,
						  VkImageLayout new_layout)
{
	VkCommandBufferAllocateInfo alloc_info = (VkCommandBufferAllocateInfo) {
    	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    	.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    	.commandPool = core->cmdpool,
   		.commandBufferCount = 1
	};

    VkCommandBuffer cmdbuf;
    vkAllocateCommandBuffers(bp->dev, &alloc_info, &cmdbuf);

    VkCommandBufferBeginInfo begin_info = (VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    	.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

    vkBeginCommandBuffer(cmdbuf, &begin_info);

	VkAccessFlags src_flags, dst_flags;
	VkPipelineStageFlags src_stage, dst_stage;
	
	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
		new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		src_flags = 0;
		dst_flags = VK_ACCESS_TRANSFER_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			 new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		src_flags = VK_ACCESS_TRANSFER_WRITE_BIT;
		dst_flags = VK_ACCESS_SHADER_READ_BIT;

		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		loge("vktrasitionimglayout(), unsupported image layout trasition\n");
		exit(0);
	}
	
	VkImageSubresourceRange subresrange = (VkImageSubresourceRange) {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	
	VkImageMemoryBarrier barrier = (VkImageMemoryBarrier) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = src_flags,
		.dstAccessMask = dst_flags,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = *image,
		.subresourceRange = subresrange
	};

	vkCmdPipelineBarrier(cmdbuf, src_stage, dst_stage, 0, 0, NULL, 0, NULL,	1, &barrier);

	vkEndCommandBuffer(cmdbuf);

	flushl();
	
	VkSubmitInfo submit_info = (VkSubmitInfo) {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = NULL,
		.pWaitDstStageMask = 0,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmdbuf,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL
	};

	UPDATE_DEBUG_LINE();
	VkResult res = vkQueueSubmit(bp->queue, 1, &submit_info, VK_NULL_HANDLE);
	assert(res == VK_SUCCESS);
	
	vkQueueWaitIdle(bp->queue);
	
	vkFreeCommandBuffers(bp->dev, core->cmdpool, 1, &cmdbuf);

	logt("image layout transiotioned succesfully\n");
}

void vkcopybuftoimg(VkTexture* texture, VkBoilerplate* bp, VkCore* core,
					VkbaVirtualBuffer* vBuffer, uint32_t width, uint32_t height)
{
	UPDATE_DEBUG_LINE();
	
	VkCommandBuffer cmdbuf;
	
	VkCommandBufferAllocateInfo cmdbuf_alloc_info = (VkCommandBufferAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = core->cmdpool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	
	UPDATE_DEBUG_LINE();
	VkResult res = vkAllocateCommandBuffers(bp->dev, &cmdbuf_alloc_info, &cmdbuf);
	assert(res == VK_SUCCESS);
	
	VkCommandBufferBeginInfo cmdbuf_begin_info = (VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = NULL
	};
	
	UPDATE_DEBUG_LINE();
	res = vkBeginCommandBuffer(cmdbuf, &cmdbuf_begin_info);
	assert(res == VK_SUCCESS);

	VkBufferImageCopy copy = (VkBufferImageCopy) {
		.bufferOffset = vBuffer->locale.offset,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		.imageOffset = { 0, 0, 0 },
		.imageExtent = { width, height, 1 }
	};

	vkCmdCopyBufferToImage(cmdbuf, vBuffer->buffer, texture->image,
						   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

	vkEndCommandBuffer(cmdbuf);
	
	VkSubmitInfo submit_info = (VkSubmitInfo) {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = NULL,
		.pWaitDstStageMask = 0,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmdbuf,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL
	};

	UPDATE_DEBUG_LINE();
	res = vkQueueSubmit(bp->queue, 1, &submit_info, VK_NULL_HANDLE);
	assert(res == VK_SUCCESS);
	
	vkQueueWaitIdle(bp->queue);
	
	vkFreeCommandBuffers(bp->dev, core->cmdpool, 1, &cmdbuf);
}
