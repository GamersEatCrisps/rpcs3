#pragma once

#include "../VulkanAPI.h"
#include "chip_class.h"
#include "pipeline_binding_table.h"
#include "memory.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace vk
{
	struct gpu_formats_support
	{
		bool d24_unorm_s8;
		bool d32_sfloat_s8;
		bool bgra8_linear;
		bool argb8_linear;
	};

	struct gpu_shader_types_support
	{
		bool allow_float64;
		bool allow_float16;
		bool allow_int8;
	};

	struct memory_type_mapping
	{
		u32 host_visible_coherent;
		u32 device_local;

		PFN_vkGetMemoryHostPointerPropertiesEXT getMemoryHostPointerPropertiesEXT;
	};

	class physical_device
	{
		VkInstance parent = VK_NULL_HANDLE;
		VkPhysicalDevice dev = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties props;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceMemoryProperties memory_properties;
		std::vector<VkQueueFamilyProperties> queue_props;

		std::unordered_map<VkFormat, VkFormatProperties> format_properties;
		gpu_shader_types_support shader_types_support{};
		VkPhysicalDeviceDriverPropertiesKHR driver_properties{};

		bool stencil_export_support = false;
		bool conditional_render_support = false;
		bool external_memory_host_support = false;
		bool unrestricted_depth_range_support = false;

		friend class render_device;
	private:
		void get_physical_device_features(bool allow_extensions);

	public:

		physical_device() = default;
		~physical_device() = default;

		void create(VkInstance context, VkPhysicalDevice pdev, bool allow_extensions);

		std::string get_name() const;

		driver_vendor get_driver_vendor() const;
		std::string get_driver_version() const;
		chip_class get_chip_class() const;

		u32 get_queue_count() const;

		VkQueueFamilyProperties get_queue_properties(u32 queue);
		VkPhysicalDeviceMemoryProperties get_memory_properties() const;
		VkPhysicalDeviceLimits get_limits() const;

		operator VkPhysicalDevice() const;
		operator VkInstance() const;
	};

	class render_device
	{
		physical_device* pgpu = nullptr;
		memory_type_mapping memory_map{};
		gpu_formats_support m_formats_support{};
		pipeline_binding_table m_pipeline_binding_table{};
		std::unique_ptr<mem_allocator_base> m_allocator;
		VkDevice dev = VK_NULL_HANDLE;

	public:
		// Exported device endpoints
		PFN_vkCmdBeginConditionalRenderingEXT cmdBeginConditionalRenderingEXT = nullptr;
		PFN_vkCmdEndConditionalRenderingEXT cmdEndConditionalRenderingEXT = nullptr;

	public:
		render_device() = default;
		~render_device() = default;

		void create(vk::physical_device& pdev, u32 graphics_queue_idx);
		void destroy();

		const VkFormatProperties get_format_properties(VkFormat format);

		bool get_compatible_memory_type(u32 typeBits, u32 desired_mask, u32* type_index) const;

		const physical_device& gpu() const;
		const memory_type_mapping& get_memory_mapping() const;
		const gpu_formats_support& get_formats_support() const;
		const pipeline_binding_table& get_pipeline_binding_table() const;
		const gpu_shader_types_support& get_shader_types_support() const;

		bool get_shader_stencil_export_support() const;
		bool get_depth_bounds_support() const;
		bool get_alpha_to_one_support() const;
		bool get_conditional_render_support() const;
		bool get_unrestricted_depth_range_support() const;
		bool get_external_memory_host_support() const;

		mem_allocator_base* get_allocator() const;

		operator VkDevice() const;
	};

	memory_type_mapping get_memory_mapping(const physical_device& dev);
	gpu_formats_support get_optimal_tiling_supported_formats(const physical_device& dev);
	pipeline_binding_table get_pipeline_binding_table(const physical_device& dev);

	extern const render_device* g_render_device;
}
