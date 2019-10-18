/**
 * Copyright (C) 2014 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/reshade#license
 */

#pragma once

#include "runtime.hpp"
#include "state_block.hpp"
#include <unordered_set>

namespace reshade { enum class texture_reference; }
namespace reshadefx { struct sampler_info; }

namespace reshade::opengl
{
	class runtime_gl : public runtime
	{
	public:
		runtime_gl();

		bool on_init(HWND hwnd, unsigned int width, unsigned int height);
		void on_reset(bool is_frame = false);
		void on_present();

		void on_draw_call(unsigned int vertices);
		void on_fbo_attachment(GLenum attachment, GLenum target, GLuint object, GLint level);
		void on_dimensions_sizing(GLfloat y, GLsizei width, GLsizei height);

		bool capture_screenshot(uint8_t *buffer) const override;

		GLuint _current_vertex_count = 0; // Used to calculate vertex count inside glBegin/glEnd pairs
		std::unordered_set<HDC> _hdcs;

		std::string _app_name = "none";
		unsigned int _runtime_vendor_id = 0;

		int _enum_app_name;
		enum APPNAME
		{
			APP_DEFAULT,
			APP_CEMU,
			APP_DOLPHIN,
			APP_PCSX2,
			APP_PPSSPP,
			APP_REDREAM,
			APP_RPCS3
		};

	private:
		struct depth_source_info
		{
			unsigned int width, height;
			GLint level, format;
			unsigned int num_drawcalls, num_vertices;
		};

		bool init_texture(texture &info) override;
		void upload_texture(texture &texture, const uint8_t *data) override;
		bool update_texture_reference(texture &texture);
		void update_texture_references(texture_reference type);

		bool compile_effect(effect_data &effect) override;
		void unload_effects() override;

		bool add_sampler(const reshadefx::sampler_info &info, struct opengl_technique_data &technique_init);
		bool init_technique(technique &info, const struct opengl_technique_data &technique_init, const std::unordered_map<std::string, GLuint> &entry_points, std::string &errors);

		void render_technique(technique &technique) override;

#if RESHADE_GUI
		void init_imgui_resources();
		void render_imgui_draw_data(ImDrawData *data) override;
		void draw_debug_menu();
#endif

		void detect_depth_source();
		void set_app_name(std::string &app_name);

		state_block _app_state;
		GLuint _depth_source = 0;
		GLuint _current_depth_buffer;
		GLuint _selected_depth_buffer;

		int _app_mode = 1;
		GLsizei _new_src_width = 0;
		GLsizei _new_src_height = 0;
		GLsizei _new_dst_width = 0;
		GLsizei _new_dst_height = 0;

		GLsizei _app_width = 0;
		GLsizei _app_height = 0;
		GLsizei _app_width_fbo = 0;
		GLsizei _app_height_fbo = 0;
		GLfloat _app_offset_y_fbo = 0;

		std::unordered_map<GLuint, depth_source_info> _depth_source_table;

		enum BUF
		{
#if RESHADE_GUI
			VBO_IMGUI,
			IBO_IMGUI,
#else
			BUF_DUMMY,
#endif
				NUM_BUF
		};
		enum TEX
		{
			TEX_BACK,
			TEX_BACK_SRGB,
			TEX_DEPTH,
				NUM_TEX
		};
		enum VAO
		{
			VAO_FX,
#if RESHADE_GUI
			VAO_IMGUI,
#endif
				NUM_VAO
		};
		enum FBO
		{
			FBO_BACK,
			FBO_DEPTH,
			FBO_BLIT,
			FBO_BLIT_WIDE_DEPTH,
				NUM_FBO
		};
		enum RBO
		{
			RBO_COLOR,
			RBO_DEPTH,
				NUM_RBO
		};

		GLuint _buf[NUM_BUF] = {};
		GLuint _tex[NUM_TEX] = {};
		GLuint _vao[NUM_VAO] = {};
		GLuint _fbo[NUM_FBO] = {};
		GLuint _rbo[NUM_RBO] = {};
#if RESHADE_GUI
		GLuint _imgui_program = 0;
		int _imgui_uniform_tex = 0;
		int _imgui_uniform_proj = 0;
#endif
		std::vector<GLuint> _reserved_texture_names;
		std::unordered_map<size_t, GLuint> _effect_sampler_states;
		std::vector<std::pair<GLuint, GLsizeiptr>> _effect_ubos;

		bool _force_main_depth_buffer = false;
		bool _is_upscaled = false;
		bool _alt_detection = false;
		bool _manual_detection = false;
		bool _is_upscale_init_done = false;
	};
}
