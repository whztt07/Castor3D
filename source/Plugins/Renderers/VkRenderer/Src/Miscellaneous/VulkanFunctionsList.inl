/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#if defined( CreateSemaphore )
#	undef CreateSemaphore
#endif

#if defined( VK_USE_PLATFORM_EXTENSIONS )
#	if defined( VK_USE_PLATFORM_WIN32_KHR )
VK_LIB_FUNCTION( CreateWin32SurfaceKHR )
#	elif defined( VK_USE_PLATFORM_XCB_KHR )
VK_LIB_FUNCTION( CreateXcbSurfaceKHR )
#	elif defined( VK_USE_PLATFORM_XLIB_KHR )
VK_LIB_FUNCTION( CreateXlibSurfaceKHR )
#	endif
#endif

VK_LIB_FUNCTION( AcquireNextImageKHR )
VK_LIB_FUNCTION( AllocateCommandBuffers )
VK_LIB_FUNCTION( AllocateDescriptorSets )
VK_LIB_FUNCTION( AllocateMemory )
VK_LIB_FUNCTION( BeginCommandBuffer )
VK_LIB_FUNCTION( BindBufferMemory )
VK_LIB_FUNCTION( BindImageMemory )
VK_LIB_FUNCTION( CmdBeginQuery )
VK_LIB_FUNCTION( CmdBeginRenderPass )
VK_LIB_FUNCTION( CmdBindDescriptorSets )
VK_LIB_FUNCTION( CmdBindIndexBuffer )
VK_LIB_FUNCTION( CmdBindPipeline )
VK_LIB_FUNCTION( CmdBindVertexBuffers )
VK_LIB_FUNCTION( CmdBlitImage )
VK_LIB_FUNCTION( CmdClearColorImage )
VK_LIB_FUNCTION( CmdCopyBuffer )
VK_LIB_FUNCTION( CmdCopyBufferToImage )
VK_LIB_FUNCTION( CmdCopyImage )
VK_LIB_FUNCTION( CmdDraw )
VK_LIB_FUNCTION( CmdDrawIndexed )
VK_LIB_FUNCTION( CmdEndQuery )
VK_LIB_FUNCTION( CmdEndRenderPass )
VK_LIB_FUNCTION( CmdExecuteCommands )
VK_LIB_FUNCTION( CmdCopyImageToBuffer )
VK_LIB_FUNCTION( CmdNextSubpass )
VK_LIB_FUNCTION( CmdPipelineBarrier )
VK_LIB_FUNCTION( CmdPushConstants )
VK_LIB_FUNCTION( CmdResetQueryPool )
VK_LIB_FUNCTION( CmdSetScissor )
VK_LIB_FUNCTION( CmdSetViewport )
VK_LIB_FUNCTION( CmdWriteTimestamp )
VK_LIB_FUNCTION( CreateBuffer )
VK_LIB_FUNCTION( CreateBufferView )
VK_LIB_FUNCTION( CreateCommandPool )
VK_LIB_FUNCTION( CreateDescriptorPool )
VK_LIB_FUNCTION( CreateDescriptorSetLayout )
VK_LIB_FUNCTION( CreateDevice )
VK_LIB_FUNCTION( CreateFence )
VK_LIB_FUNCTION( CreateFramebuffer )
VK_LIB_FUNCTION( CreateGraphicsPipelines )
VK_LIB_FUNCTION( CreateImage )
VK_LIB_FUNCTION( CreateImageView )
VK_LIB_FUNCTION( CreateInstance )
VK_LIB_FUNCTION( CreatePipelineLayout )
VK_LIB_FUNCTION( CreateRenderPass )
VK_LIB_FUNCTION( CreateQueryPool )
VK_LIB_FUNCTION( CreateSampler )
VK_LIB_FUNCTION( CreateSemaphore )
VK_LIB_FUNCTION( CreateShaderModule )
VK_LIB_FUNCTION( CreateSwapchainKHR )
VK_LIB_FUNCTION( DestroyBuffer )
VK_LIB_FUNCTION( DestroyBufferView )
VK_LIB_FUNCTION( DestroyCommandPool )
VK_LIB_FUNCTION( DestroyDescriptorPool )
VK_LIB_FUNCTION( DestroyDescriptorSetLayout )
VK_LIB_FUNCTION( DestroyDevice )
VK_LIB_FUNCTION( DestroyFence )
VK_LIB_FUNCTION( DestroyFramebuffer )
VK_LIB_FUNCTION( DestroyImage )
VK_LIB_FUNCTION( DestroyImageView )
VK_LIB_FUNCTION( DestroyInstance )
VK_LIB_FUNCTION( DestroyPipeline )
VK_LIB_FUNCTION( DestroyPipelineLayout )
VK_LIB_FUNCTION( DestroyQueryPool )
VK_LIB_FUNCTION( DestroyRenderPass )
VK_LIB_FUNCTION( DestroySampler )
VK_LIB_FUNCTION( DestroySemaphore )
VK_LIB_FUNCTION( DestroyShaderModule )
VK_LIB_FUNCTION( DestroySurfaceKHR )
VK_LIB_FUNCTION( DestroySwapchainKHR )
VK_LIB_FUNCTION( DeviceWaitIdle )
VK_LIB_FUNCTION( EndCommandBuffer )
VK_LIB_FUNCTION( EnumerateDeviceExtensionProperties )
VK_LIB_FUNCTION( EnumerateInstanceExtensionProperties )
VK_LIB_FUNCTION( EnumerateInstanceLayerProperties )
VK_LIB_FUNCTION( EnumeratePhysicalDevices )
VK_LIB_FUNCTION( FlushMappedMemoryRanges )
VK_LIB_FUNCTION( FreeCommandBuffers )
VK_LIB_FUNCTION( FreeDescriptorSets )
VK_LIB_FUNCTION( FreeMemory )
VK_LIB_FUNCTION( GetBufferMemoryRequirements )
VK_LIB_FUNCTION( GetDeviceProcAddr )
VK_LIB_FUNCTION( GetDeviceQueue )
VK_LIB_FUNCTION( GetImageMemoryRequirements )
VK_LIB_FUNCTION( GetImageSubresourceLayout )
VK_LIB_FUNCTION( GetPhysicalDeviceFeatures )
VK_LIB_FUNCTION( GetPhysicalDeviceFormatProperties )
VK_LIB_FUNCTION( GetPhysicalDeviceMemoryProperties )
VK_LIB_FUNCTION( GetPhysicalDeviceProperties )
VK_LIB_FUNCTION( GetPhysicalDeviceQueueFamilyProperties )
VK_LIB_FUNCTION( GetPhysicalDeviceSurfaceCapabilitiesKHR )
VK_LIB_FUNCTION( GetPhysicalDeviceSurfaceFormatsKHR )
VK_LIB_FUNCTION( GetPhysicalDeviceSurfacePresentModesKHR )
VK_LIB_FUNCTION( GetPhysicalDeviceSurfaceSupportKHR )
VK_LIB_FUNCTION( GetQueryPoolResults )
VK_LIB_FUNCTION( GetSwapchainImagesKHR )
VK_LIB_FUNCTION( MapMemory )
VK_LIB_FUNCTION( QueuePresentKHR )
VK_LIB_FUNCTION( QueueSubmit )
VK_LIB_FUNCTION( QueueWaitIdle )
VK_LIB_FUNCTION( ResetCommandBuffer )
VK_LIB_FUNCTION( ResetFences )
VK_LIB_FUNCTION( UnmapMemory )
VK_LIB_FUNCTION( UpdateDescriptorSets )
VK_LIB_FUNCTION( WaitForFences )

#undef VK_LIB_FUNCTION
