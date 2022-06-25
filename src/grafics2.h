#ifndef GRAFICS2_H
#define GRAFICS2_H

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <vulkan.h>
#include <windows.h>

#define KILOBYTE 1024
#define MEGABYTE 1024*1024
#define GIGABYTE 1024*1024*1024

// ---------------------------------------------------------------------------------
/*
  		utils.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	char r;
	char g;
	char b;
	char a;
} pixel;

typedef long long 			i64;
typedef unsigned long long 	u64;
typedef int 				i32;
typedef unsigned int 		u32;
typedef short 				i16;
typedef unsigned short 		u16;
typedef char	 			i8;
typedef unsigned char 		u8;

// ---------------------------------------------------------------------------------
/*
  		sort.c
 */
// ---------------------------------------------------------------------------------

// bubble sort for 'signed short'-type, aka. int16_t
void bubble_sorts(short* arr, uint64_t size);
// bubble sort for 'unsigned int'-type, aka. uint32_t
void bubble_sortui(uint32_t* arr, uint64_t size);
// TODO: add faster and better sort algorithms

// ---------------------------------------------------------------------------------
/*
  		array.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	uint32_t		stride;
	uint32_t 		alloc_size;
	uint32_t 		size;
	void* 			data;
} Array;

void arr_init(Array* arr, uint64_t stride);
void arr_add(Array* arr, void* src);
void arr_insert();								// TODO
void arr_insertm();								// TODO: insert multiple
void arr_push(Array* arr);
void arr_remove(Array* arr, uint32_t index);
void arr_duplicates(Array* arr);
void arr_pop(Array* arr);
void arr_clean(Array* arr);
void arr_free(Array* arr);
uint64_t arr_sizeof(Array* arr);
void arr_copy(Array* arr, void* src, uint32_t count);
void* arr_get(Array* arr, uint32_t index);
// void arr_sort(Array* arr);

// ---------------------------------------------------------------------------------
/*
  		memory.c
 */
// ---------------------------------------------------------------------------------

// variadic malloc
// vmalloc(size, &ptr, size, &ptr);
// void vmalloc(...);		// TODO

// variadic free
// vfree(arr1, arr2, arr3, ...);
// void vfree(...);		// TODO

// ---------------------------------------------------------------------------------
/*
  		logger.c
 */
// ---------------------------------------------------------------------------------

typedef enum { LOG_INFO, LOG_DEBUG, LOG_TRACE, LOG_WARNING, LOG_ERROR } log_levels;

void log_init(const char* file);
void glog(int level, const char* file, int line, const char* format, ...);
void flushl();		// flush logs
void log_close();

#define logi(...) glog(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define logd(...) glog(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define logt(...) glog(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define logw(...) glog(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define loge(...) glog(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

// ---------------------------------------------------------------------------------
/*
  		fileio.c
 */
// ---------------------------------------------------------------------------------

char* file_read(const char* path, uint32_t* file_size);
void file_free(void* buffer);

// ---------------------------------------------------------------------------------
/*
  		bmploader.c
 */
// ---------------------------------------------------------------------------------

char* bmp_load(const char* file, uint32_t* width, uint32_t* height);
void bmp_free(char* buffer);

// ---------------------------------------------------------------------------------
/*
  		ttfparser.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	uint32_t scaler_type;
	uint16_t numTables;
	uint16_t searchRange;
	uint16_t entrySelector;
	uint16_t rangeShift;
} ttf_table_directory;

typedef struct
{
	uint32_t tag;
	uint32_t checkSum;
	uint32_t offset;
	uint32_t length;
} ttf_header;

typedef struct
{
	uint32_t version;
	uint32_t font_revision;
	uint32_t checksum_adjustment;
	uint32_t magic_number;
	uint16_t flags;
	uint16_t units_per_em;
	int64_t created;
	int64_t modified;
	int16_t x_min;
	int16_t y_min;
	int16_t x_max;
	int16_t y_max;
	uint16_t mac_style;
	uint16_t lowest_rec_ppem;
	int16_t font_direction_hint;
	int16_t index_to_loc_format;
	int16_t glyph_data_format;
} ttf_head;

typedef struct
{
	uint32_t version;
	int16_t ascent;
	int16_t descent;
	int16_t line_gap;
	uint16_t advance_with_max;
	int16_t min_left_side_bearing;
	int16_t min_right_side_bearing;
	int16_t x_max_extent;
	int16_t caret_slope_rise;
	int16_t caret_slope_run;
	int16_t caret_offset;
	int16_t reserved[4];
	int16_t metric_date_format;
	uint16_t num_of_long_hor_metrics;
} ttf_hhea;

typedef struct
{
	uint32_t version;
	uint16_t num_glyphs;
	uint16_t max_points;
	uint16_t max_contours;
	uint16_t max_component_points;
	uint16_t max_component_contours;
	uint16_t max_zones;
	uint16_t max_twilight_points;
	uint16_t max_storage;
	uint16_t max_function_defs;
	uint16_t max_instruction_defs;
	uint16_t max_stack_elements;
	uint16_t max_size_of_instructions;
	uint16_t max_component_elements;
	uint16_t max_component_depth;
} ttf_maxp;

typedef struct
{
	uint16_t advance_width;
	int16_t left_side_bearing;
} ttf_long_hor_metric;

typedef struct
{
	ttf_long_hor_metric* h_metrics;
	int16_t* left_side_bearing;
} ttf_hmtx;

typedef struct
{
	uint16_t version;
	uint16_t number_subtables;
} ttf_cmap_index;

typedef struct
{
	uint16_t platform_id;
	uint16_t platform_specific_id;
	uint32_t offset;
} ttf_cmap_estable;

typedef struct
{
	uint16_t format;
	uint16_t length;
	uint16_t language;
	uint16_t seg_count_2;
	uint16_t search_range;
	uint16_t entry_selector;
	uint16_t range_shift;
	uint16_t* end_code;
	uint16_t reserved_pad;
	uint16_t* start_code;
	uint16_t* id_delta;
	uint16_t* id_range_offset;
	uint16_t* glyph_index_array;
} ttf_cmap_format4;

typedef struct
{
	ttf_cmap_index index;
	ttf_cmap_estable* tables;
	ttf_cmap_format4 format;
} ttf_cmap;

typedef struct
{
	int16_t num_of_contours;
	int16_t	x_min;
	int16_t	y_min;
	int16_t	x_max;
	int16_t	y_max;
} ttf_glyf_header;

typedef struct
{
	// uint16_t num_contours;
	uint16_t* end_contours;
	uint16_t instruction_len;
	uint8_t* instructions;
	uint16_t num_points;
	uint8_t* flags;
	int16_t* px;
	int16_t* py;
} ttf_glyf_data;

typedef struct
{
	ttf_glyf_header header;
	ttf_glyf_data data;
} ttf_glyf;

typedef struct
{	
	ttf_head* head;
	ttf_hhea* hhea;
	ttf_maxp* maxp;
	ttf_hmtx* hmtx;
	ttf_cmap* cmap;
	uint32_t* loca;
	ttf_glyf* glyf;
} ttf_core;

typedef struct
{
	float x;
	float y;
	uint8_t flag;
} ttf_vector2f;

// cmap, glyf, head, hhea, hmtx, loca, maxp are MANDATORY
// everything else is just additions

void ttf_load(ttf_core* ttf);
void ttf_free(ttf_core* ttf);

void ttf_head_load(ttf_core* ttf, void* file);
void ttf_hhea_load(ttf_core* ttf, void* file);
void ttf_maxp_load(ttf_core* ttf, void* file);
void ttf_hmtx_load(ttf_core* ttf, void* file);
void ttf_cmap_load(ttf_core* ttf, void* file);
void ttf_loca_load(ttf_core* ttf, void* file);
void ttf_glyf_load(ttf_core* ttf, void* file); // TODO

void ttf_glyf_data_load(ttf_core* ttf, ttf_glyf* glyf, void* file, uint32_t offset);
void ttf_cmap_format4_table_load(ttf_cmap* cmap, void* file);

void ttf_hmtx_free(ttf_core* ttf);
void ttf_cmap_free(ttf_core* ttf);
void ttf_loca_free(ttf_core* ttf);
void ttf_glyf_data_free(ttf_glyf* glyf);
void ttf_glyf_free(ttf_core* ttf);

int ttf_glyph_index_get(ttf_core* ttf, u16 code_point);
void* ttf_to_bmp(char ch, uint32_t width, uint32_t height, ttf_core* ttf);
void* ttf_textureatlas_generate();		// TODO

// ---------------------------------------------------------------------------------
/*
  		win32.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	HINSTANCE hinstance;
	HWND hwnd;
	bool should_close;
} Window;

void windowc(Window* win, const char* name, const uint32_t width, const uint32_t height);
void windowd(Window* win);
void wpoll_events(Window* win);
void wframebuffer_size(Window* win, uint32_t* width, uint32_t* height);

// ---------------------------------------------------------------------------------
/*
  		vkdebug.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	int line;
	const char* file;
} dUserData;

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *user_data
	 );

// ---------------------------------------------------------------------------------
/*
  		vkboilerplate.c
 */
// ---------------------------------------------------------------------------------

PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXTproxy;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXTproxy;

typedef struct
{
	VkInstance inst;
	VkDebugUtilsMessengerEXT dmessenger;
	dUserData user_data;
	VkSurfaceKHR surface;
	VkPhysicalDevice phydev;
	VkDevice dev;
	VkQueue queue;
} VkBoilerplate;

void vkloadextensions(VkBoilerplate* bp);
void vkboilerplatec(VkBoilerplate* bp, Window* win);
void vkboilerplated(VkBoilerplate* bp);

// ---------------------------------------------------------------------------------
/*
  		vkcore.c
 */
// ---------------------------------------------------------------------------------

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct
{
	VkSwapchainKHR swapchain;
	VkExtent2D swpcext;				// swapchain extent
	uint32_t swpcimg_count;			// swapchain image count
	VkImage* swpcimgs;
	VkImageView* swpcviews;
	VkRenderPass renderpass;
	VkFramebuffer* framebuffers;
	VkCommandPool cmdpool;
	VkCommandBuffer* cmdbuffers;
	VkSemaphore* img_avb;
	VkSemaphore* render_fin;
	VkFence* in_flight;
} VkCore;

void vkcorec(VkCore* core, VkBoilerplate* bp, Window* win);
void vkcored(VkCore* core, VkBoilerplate* bp);

// ---------------------------------------------------------------------------------
/*
  		vkma_allocator.c
 */
// ---------------------------------------------------------------------------------

// TODO: comments

typedef struct VkmaSubAllocation_t
{
	u64 size;
	u64 offset;
} VkmaSubAllocation;

typedef struct VkmaAllocation_t
{
	u32 heapIndex;
	u32 blockIndex;
	VkmaSubAllocation locale;
	VkDeviceMemory memoryCopy;
	void* ptr;
	char name[64];
} VkmaAllocation;

typedef struct VkmaBlock_t
{
	u64 size;
	u64 availableSize;
	VkDeviceMemory memory;
	Array freeChunks;			// VkmaSubAllocation
	void* ptr;
} VkmaBlock;

typedef struct VkmaHeap_t
{
	Array blocks;		// VkmaBlock
} VkmaHeap;

typedef struct VkmaAllocator_t
{
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkPhysicalDeviceMemoryProperties phdmProps;
	VkmaHeap* heaps;
} VkmaAllocator;

typedef struct VkmaAllocatorCreateInfo_t
{
	VkPhysicalDevice physicalDevice;
	VkDevice device;
} VkmaAllocatorCreateInfo;

typedef enum VkmaAllocationUsageFlagBits_t
{
	VKMA_ALLOCATION_USAGE_AUTO = 0x01,
	VKMA_ALLOCATION_USAGE_HOST = 0x02,
	VKMA_ALLOCATION_USAGE_DEVICE = 0x04
	// VKMA_ALLOCATION_USAGE_AUTO_PREFER_HOST = 0x08,		// not implmented
	// VKMA_ALLOCATION_USAGE_AUTO_PREFER_DEVICE = 0x10		// not implmented
} VkmaAllocationUsageFlagBits;
typedef VkFlags VkmaAllocationUsageFlags;

typedef enum VkmaAllocationCreateFlagBits_t
{
	VKMA_ALLOCATION_CREATE_MAPPED = 0x01,
	VKMA_ALLOCATION_CREATE_DONT_CREATE = 0x02,
	VKMA_ALLOCATION_CREATE_DONT_BIND = 0x04,
	VKMA_ALLOCATION_CREATE_DONT_ALLOCATE = 0x08,
	VKMA_ALLOCATION_CREATE_HOST_RANDOM_ACCESS = 0x10,	// for multithreaded stuff
	VKMA_ALLOCATION_CREATE_HOST_SEQUENTIAL_WRITE = 0x20
} VkmaAllocationCreateFlagBits;
typedef VkFlags VkmaAllocationCreateFlags;

typedef struct VkmaAllocation_t VkmaAllocation;
typedef struct VkmaAllocationInfo_t
{
	VkmaAllocationUsageFlags usage;
	VkmaAllocationCreateFlags create;
	const char* name;
	void* pMappedPtr;
} VkmaAllocationInfo;

VkResult vkmaCreateAllocator(VkmaAllocator* allocator, VkmaAllocatorCreateInfo* info);
VkResult vkmaCreateBuffer(VkmaAllocator* allocator,
						  VkBufferCreateInfo* bufferInfo, VkBuffer* buffer,
						  VkmaAllocationInfo* allocInfo, VkmaAllocation* allocation);
VkResult vkmaCreateImage(VkmaAllocator* allocator,
						 VkImageCreateInfo* imageInfo, VkImage* image,
						 VkmaAllocationInfo* allocInfo, VkmaAllocation* allocation);

VkResult vkmaMapMemory(VkmaAllocator* allocator, VkmaAllocation* allocation, void** ptr);
void vkmaUnmapMemory(VkmaAllocator* allocator, VkmaAllocation* allocation);

void vkmaDestroyAllocator(VkmaAllocator* allocator);
void vkmaDestroyBuffer(VkmaAllocator* allocator, VkBuffer* buffer,
					   VkmaAllocation* allocation);
void vkmaDestroyImage(VkmaAllocator* allocator, VkImage* image, VkmaAllocation* allocation);

// ---------------------------------------------------------------------------------
/*
  		vkba_allocator.c
 */
// ---------------------------------------------------------------------------------

#define HOST_INDEX 0
#define DEVICE_INDEX 1

typedef struct VkbaPage_t
{
	VkBuffer buffer;
	VkmaAllocation allocation;
	void* ptr;
	Array freeSubAllocs;		// VmkaSubAllocation
} VkbaPage;

typedef struct VkbaAllocator_t
{
	VkbaPage* pages; 		// first is HOST, second is DEVICE
	u64 uniformBufferOffsetAlignment;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
} VkbaAllocator;

typedef struct VkbaAllocatorCreateInfo_t
{
	VkmaAllocator* allocator;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
} VkbaAllocatorCreateInfo;

typedef struct VkbaVirtualBuffer_t
{
	u32 pageIndex;
	VkBuffer buffer;
	VkmaSubAllocation locale;
	u64 range;
	void* src;
	void* dst;
} VkbaVirtualBuffer;

typedef enum VkbaVirtualBufferType_t
{
	VKBA_VIRTUAL_BUFFER_TYPE_UNIFORM = 1
	// VKBA_VIRTUAL_BUFFER_TYPE_STORAGE = 2
} VkbaVirtualBufferType;

typedef struct VkbaVirtualBufferInfo_t
{
	u32 index;
	u64 size;
	void* src;
	VkbaVirtualBufferType type;
} VkbaVirtualBufferInfo;

void vkbaCreateAllocator(VkbaAllocator* bAllocator, VkbaAllocatorCreateInfo* info);
void vkbaDestroyAllocator(VkbaAllocator* bAllocator, VkmaAllocator* allocator);
VkResult vkbaCreateVirtualBuffer(VkbaAllocator* bAllocator, VkbaVirtualBuffer* buffer,
								 VkbaVirtualBufferInfo* info);
VkResult vkbaStageVirtualBuffer(VkbaAllocator* bAllocator, VkbaVirtualBuffer* srcBuffer,
								VkbaVirtualBufferInfo* info);
void vkbaDestroyVirtualBuffer(VkbaAllocator* bAllocator, VkbaVirtualBuffer* buffer);

// ---------------------------------------------------------------------------------
/*
  		VULKAN DESCRIPTOR SET MANAGER
  		vkds_manager.c
 */
// ---------------------------------------------------------------------------------

typedef struct VkdsManager_t
{
	VkDevice deviceCopy;
	VkDescriptorPool descPool;
	Array descSetLayouts;		// VkDescriptorSetLayout
} VkdsManager;

typedef enum VkdsManagerCreateFlagBits_t
{
	VKDS_MANAGER_CREATE_POOL_ALLOW_FREE_BIT = 0x00000001
} VkdsManagerCreateFlagBits;
typedef VkFlags VkdsManagerCreateFlags;

typedef struct VkdsManagerCreateInfo_t
{
	VkDevice device;
	u32 poolSize;
	VkdsManagerCreateFlags flags;
} VkdsManagerCreateInfo;

typedef enum VkdsBindingType_t
{
	VKDS_BINDING_TYPE_IMAGE_SAMPLER = 1,
	VKDS_BINDING_TYPE_UNIFORM_BUFFER = 2
} VkdsBindingType;
typedef VkFlags VkdsBindingTypeFlags;

typedef enum VkdsBindingStage_t
{
	VKDS_BINDING_STAGE_VERTEX = 1,
	VKDS_BINDING_STAGE_FRAGMENT = 2
} VkdsBindingStage;
typedef VkFlags VkdsBindingStageFlags;

typedef struct VkdsBindingImageSamplerData_t
{
	VkSampler sampler;
	VkImageView view;
} VkdsBindingImageSamplerData;

typedef struct VkdsBindingUniformData_t
{
	VkbaVirtualBuffer* vbuffers;
} VkdsBindingUniformData;

typedef union VkdsBindingData_t
{
	VkdsBindingImageSamplerData imageSampler;
	VkdsBindingUniformData uniformBuffer;
} VkdsBindingData;

typedef struct VkdsBinding_t
{
	u32 location;
	VkdsBindingTypeFlags type;
	VkdsBindingStageFlags stage;
	VkdsBindingData data;
} VkdsBinding;

typedef struct VkdsDescriptorSetCreateInfo_t
{
	u32 bindingCount;
	VkdsBinding* bindings;
	u32 framesInFlight;
} VkdsDescriptorSetCreateInfo;

VkResult vkdsCreateManager(VkdsManager* manager, VkdsManagerCreateInfo* info);
VkResult vkdsCreateDescriptorSets(VkdsManager* manager, VkdsDescriptorSetCreateInfo* info,
								  VkDescriptorSet* descriptorSets,
								  VkDescriptorSetLayout* outDescSetLayout);
void vkdsDestroyManager(VkdsManager* manager);

// ---------------------------------------------------------------------------------
/*
  		vktexture.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	VkImage image;
	VkmaAllocation allocation;
	VkImageView view;
	VkSampler sampler;
} VkTexture;

void vktexturec(VkTexture* texture, VkBoilerplate* bp, VkCore* core,
				VkmaAllocator* mAllocator, VkbaAllocator* bAllocator);
void vktextured(VkTexture* texture, VkBoilerplate* bp, VkmaAllocator* mAllocator);
void vktransitionimglayout(VkImage* image, VkBoilerplate* bp, VkCore* core,
						  VkFormat format, VkImageLayout old_layout,
						  VkImageLayout new_layout);
void vkcopybuftoimg(VkTexture* texture, VkBoilerplate* bp, VkCore* core,
					VkbaVirtualBuffer* vBuffer, uint32_t width, uint32_t height);

// ---------------------------------------------------------------------------------
/*
  		vkdoodad.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	float position[2];
	float uv[2];
} Vertex;

typedef struct
{
	VkPipeline pipeline;
	VkPipelineLayout pipeline_layout;
	VkbaVirtualBuffer vertexbuff;
	VkbaVirtualBuffer indexbuff;
	
	VkTexture texture;
	float uboData[3];
	VkbaVirtualBuffer ubos[MAX_FRAMES_IN_FLIGHT];

	VkDescriptorSetLayout dlayout;
	VkDescriptorSet dsets[MAX_FRAMES_IN_FLIGHT];
} VkDoodad;

void vkdoodadc(VkDoodad* doodad, VkbaAllocator* bAllocator, VkmaAllocator* mAllocator, 
			   VkCore* core, VkBoilerplate* bp, VkdsManager* dsManager);
void vkdoodadd(VkDoodad* doodad, VkbaAllocator* bAllocator,
			   VkBoilerplate* bp, VkmaAllocator* mAllocator);
void vkdoodadb(VkDoodad* doodad, VkCommandBuffer cmdbuf, uint32_t current_frame);

// ---------------------------------------------------------------------------------
/*
  		VULKAN BINDING PIPELINE MACHINE
  		vkbp_machine.c
 */
// ---------------------------------------------------------------------------------

typedef struct VkbpMachine_t
{
	void* pool;
	u64 totalSize;
	u64 availableSize;
} VkbpMachine;

typedef enum VkbpInstruction_t
{
	VKBP_INSTRUCTION_BIND_PIPELINE = 0x0001,
	VKBP_INSTRUCTION_BIND_VERTEX_BUFFER = 0x0002,
	VKBP_INSTRUCTION_BIND_INDEX_BUFFER = 0x0004,
	VKBP_INSTRUCTION_BIND_DESCRIPTOR_SETS = 0x0008,
	VKBP_INSTRUCTION_DRAW_INDEXED = 0x0010,
	VKBP_INSTRUCTION_START_PIPELINE = 0x0020,
	VKBP_INSTRUCTION_END_PIPELINE = 0x0040
} VkbpInstruction;
typedef u16 VkbpInstructionFlag;

typedef struct VkbpBindingPipelineInfo_t
{
	/*
		TODO:
		 - implementation for other draws
		 - implementation for multiple vertex buffers
		 - implementation for instancing buffer
	 */
	
	VkPipeline pipeline;
	VkbaVirtualBuffer* vertexBuffer;
	VkbaVirtualBuffer* indexBuffer;
	VkPipelineLayout pipelineLayout;
	u32 descriptorSetCount;
	VkDescriptorSet* descriptorSets;
	u32 indexCount;
} VkbpBindingPipelineInfo;

VkResult vkbpCreateMachine(VkbpMachine* machine, u64 size);
void vkbpDestroyMachine(VkbpMachine* machine);
void* vkbpAddBindingPipeline(VkbpMachine* machine, VkbpBindingPipelineInfo* info);
VkResult vkbpBindBindingPipelines(VkbpMachine* machine, VkCommandBuffer cmd, u32 frame,
								  u64 offset);

// ---------------------------------------------------------------------------------
/*
  		vkapp.c
 */
// ---------------------------------------------------------------------------------

typedef struct
{
	VkBoilerplate boilerplate;
	VkCore core;
	VkmaAllocator memory_allocator;
	VkbaAllocator buffer_allocator;
	VkdsManager dsManager;
	VkDoodad doodad;
	uint32_t current_frame;
} VkApp;

void vkappc(VkApp* app, Window* window);
void vkappd(VkApp* app);
void vkrender(VkApp* app);

#endif
