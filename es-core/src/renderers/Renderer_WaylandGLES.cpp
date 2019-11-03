// Wayland Code adapted from Public Domain code available at https://github.com/eyelash/tutorials/blob/master/wayland-egl.c

#if defined(USE_WAYLANDGLES)

#include "renderers/Renderer.h"
#include "renderers/SDL2_Wayland_Input.h"
#include "Log.h"
#include "Settings.h"

#include <GLES/gl.h>

#include <wayland-egl.h>
#include <EGL/egl.h>


namespace Renderer
{
	static struct wl_display *display = nullptr;
	static struct wl_registry *registry = nullptr;
	static struct wl_compositor *compositor = nullptr;
	static struct wl_shell *shell = nullptr;
	static EGLDisplay egl_display = 0;

	struct window {
		EGLContext egl_context;
		struct wl_surface *surface;
		struct wl_shell_surface *shell_surface;
		struct wl_egl_window *egl_window;
		EGLSurface egl_surface;
		EGLConfig config;
	} *window;

	// listeners
	static void registry_add_object (void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
		if (strcmp(interface,"wl_compositor") == 0) {
			compositor = (struct wl_compositor*)wl_registry_bind (registry, name, &wl_compositor_interface, 1);
		}
		else if (strcmp(interface,"wl_shell") == 0) {
			shell = (struct wl_shell*)wl_registry_bind (registry, name, &wl_shell_interface, 1);
		}
		else if (strcmp(interface,"wl_seat") == 0) {
			seat = (struct wl_seat*)wl_registry_bind (registry, name, &wl_seat_interface, 1);
			wl_seat_add_listener(seat, &seat_listener, NULL);
		}
	}
	static void registry_remove_object (void *data, struct wl_registry *registry, uint32_t name) {

	}
	static struct wl_registry_listener registry_listener = {&registry_add_object, &registry_remove_object};

	static void shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
		wl_shell_surface_pong (shell_surface, serial);
	}
	static void shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
		if(window)
			wl_egl_window_resize (window->egl_window, width, height, 0, 0);
	}
	static void shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface) {

	}
	static struct wl_shell_surface_listener shell_surface_listener = {&shell_surface_ping, &shell_surface_configure, &shell_surface_popup_done};

	static int  windowWidth        = 0;
	static int  windowHeight       = 0;
	static int  screenWidth        = 0;
	static int  screenHeight       = 0;
	static int  screenOffsetX      = 0;
	static int  screenOffsetY      = 0;
	static int  screenRotate       = 0;
	static bool initialCursorState = 0;

	bool createWindowWayland()
	{
		LOG(LogInfo) << "Creating window...";

		if(SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			LOG(LogError) << "Error initializing SDL!\n	" << SDL_GetError();
			return false;
		}

		SDL_DisplayMode dispMode;
		SDL_GetDesktopDisplayMode(0, &dispMode);
		windowWidth   = Settings::getInstance()->getInt("WindowWidth")   ? Settings::getInstance()->getInt("WindowWidth")   : dispMode.w;
		windowHeight  = Settings::getInstance()->getInt("WindowHeight")  ? Settings::getInstance()->getInt("WindowHeight")  : dispMode.h;
		screenWidth   = Settings::getInstance()->getInt("ScreenWidth")   ? Settings::getInstance()->getInt("ScreenWidth")   : windowWidth;
		screenHeight  = Settings::getInstance()->getInt("ScreenHeight")  ? Settings::getInstance()->getInt("ScreenHeight")  : windowHeight;
		screenOffsetX = Settings::getInstance()->getInt("ScreenOffsetX") ? Settings::getInstance()->getInt("ScreenOffsetX") : 0;
		screenOffsetY = Settings::getInstance()->getInt("ScreenOffsetY") ? Settings::getInstance()->getInt("ScreenOffsetY") : 0;
		screenRotate  = Settings::getInstance()->getInt("ScreenRotate")  ? Settings::getInstance()->getInt("ScreenRotate")  : 0;

		xkb_context = xkb_context_new((xkb_context_flags)0);
		window = new struct window;
		display = wl_display_connect (NULL);
		registry = wl_display_get_registry (display);
		wl_registry_add_listener (registry, &registry_listener, NULL);
		wl_display_roundtrip (display);
		wl_display_roundtrip (display);

		egl_display = eglGetDisplay (display);
		eglInitialize (egl_display, NULL, NULL);

		setupWindow();

		window->surface = wl_compositor_create_surface (compositor);
		window->shell_surface = wl_shell_get_shell_surface (shell, window->surface);
		wl_shell_surface_add_listener (window->shell_surface, &shell_surface_listener, window);
		wl_shell_surface_set_toplevel (window->shell_surface);
		window->egl_window = wl_egl_window_create (window->surface, windowWidth, windowHeight);
		window->egl_surface = eglCreateWindowSurface (egl_display, window->config, window->egl_window, NULL);
		wl_display_dispatch_pending (display);

		LOG(LogInfo) << "Created window successfully.";

		createContext();

		return true;

	} // createWindow

	void destroyWindowWayland()
	{
		destroyContext();

		//Destroy Window
		wl_egl_window_destroy (window->egl_window);
		wl_shell_surface_destroy (window->shell_surface);
		wl_surface_destroy (window->surface);
		delete window;

		//End Display
		eglTerminate (egl_display);
		eglReleaseThread();
		destroy_wayland_input();
		wl_shell_destroy(shell);
		wl_compositor_destroy(compositor);
		wl_seat_destroy(seat);
		wl_registry_destroy(registry);
		wl_display_flush(display);
		wl_display_disconnect (display);

		//Reset all global variables
		display = nullptr;
		registry = nullptr;
		compositor = nullptr;
		seat = nullptr;
		shell = nullptr;
		egl_display = 0;
		window = nullptr;

		SDL_Quit();

	} // destroyWindow

	static GLenum convertBlendFactor(const Blend::Factor _blendFactor)
	{
		switch(_blendFactor)
		{
			case Blend::ZERO:                { return GL_ZERO;                } break;
			case Blend::ONE:                 { return GL_ONE;                 } break;
			case Blend::SRC_COLOR:           { return GL_SRC_COLOR;           } break;
			case Blend::ONE_MINUS_SRC_COLOR: { return GL_ONE_MINUS_SRC_COLOR; } break;
			case Blend::SRC_ALPHA:           { return GL_SRC_ALPHA;           } break;
			case Blend::ONE_MINUS_SRC_ALPHA: { return GL_ONE_MINUS_SRC_ALPHA; } break;
			case Blend::DST_COLOR:           { return GL_DST_COLOR;           } break;
			case Blend::ONE_MINUS_DST_COLOR: { return GL_ONE_MINUS_DST_COLOR; } break;
			case Blend::DST_ALPHA:           { return GL_DST_ALPHA;           } break;
			case Blend::ONE_MINUS_DST_ALPHA: { return GL_ONE_MINUS_DST_ALPHA; } break;
			default:                         { return GL_ZERO;                }
		}

	} // convertBlendFactor

	static GLenum convertTextureType(const Texture::Type _type)
	{
		switch(_type)
		{
			case Texture::RGBA:  { return GL_RGBA;  } break;
			case Texture::ALPHA: { return GL_ALPHA; } break;
			default:             { return GL_ZERO;  }
		}

	} // convertTextureType

	unsigned int convertColor(const unsigned int _color)
	{
		// convert from rgba to abgr
		unsigned char r = ((_color & 0xff000000) >> 24) & 255;
		unsigned char g = ((_color & 0x00ff0000) >> 16) & 255;
		unsigned char b = ((_color & 0x0000ff00) >>  8) & 255;
		unsigned char a = ((_color & 0x000000ff)      ) & 255;

		return ((a << 24) | (b << 16) | (g << 8) | (r));

	} // convertColor

	unsigned int getWindowFlags()
	{
		return SDL_WINDOW_OPENGL;

	} // getWindowFlags

	void setupWindow()
	{
		eglBindAPI (EGL_OPENGL_ES_API);
		EGLint attributes[] =
		{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
			EGL_NONE
		};
		EGLint num_config;
		eglChooseConfig (egl_display, attributes, &window->config, 1, &num_config);

	} // setupWindow

	void createContext()
	{
		window->egl_context = eglCreateContext (egl_display, window->config, EGL_NO_CONTEXT, NULL);
		eglMakeCurrent (egl_display, window->egl_surface, window->egl_surface, window->egl_context);

		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

		std::string glExts = (const char*)glGetString(GL_EXTENSIONS);
		LOG(LogInfo) << "Checking available OpenGL extensions...";
		LOG(LogInfo) << " ARB_texture_non_power_of_two: " << (glExts.find("ARB_texture_non_power_of_two") != std::string::npos ? "ok" : "MISSING");

	} // createContext

	void destroyContext()
	{
		eglDestroyContext (egl_display, window->egl_context);

	} // destroyContext

	unsigned int createTexture(const Texture::Type _type, const bool _linear, const bool _repeat, const unsigned int _width, const unsigned int _height, void* _data)
	{
		const GLenum type = convertTextureType(_type);
		unsigned int texture;

		glGenTextures(1, &texture);
		bindTexture(texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _linear ? GL_LINEAR : GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _linear ? GL_LINEAR : GL_NEAREST);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, type, _width, _height, 0, type, GL_UNSIGNED_BYTE, _data);

		return texture;

	} // createTexture

	void destroyTexture(const unsigned int _texture)
	{
		glDeleteTextures(1, &_texture);

	} // destroyTexture

	void updateTexture(const unsigned int _texture, const Texture::Type _type, const unsigned int _x, const unsigned _y, const unsigned int _width, const unsigned int _height, void* _data)
	{
		bindTexture(_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, _x, _y, _width, _height, convertTextureType(_type), GL_UNSIGNED_BYTE, _data);
		bindTexture(0);

	} // updateTexture

	void bindTexture(const unsigned int _texture)
	{
		glBindTexture(GL_TEXTURE_2D, _texture);

		if(_texture == 0) glDisable(GL_TEXTURE_2D);
		else              glEnable(GL_TEXTURE_2D);

	} // bindTexture

	void drawLines(const Vertex* _vertices, const unsigned int _numVertices, const Blend::Factor _srcBlendFactor, const Blend::Factor _dstBlendFactor)
	{
		glEnable(GL_BLEND);
		glBlendFunc(convertBlendFactor(_srcBlendFactor), convertBlendFactor(_dstBlendFactor));

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(  2, GL_FLOAT,         sizeof(Vertex), &_vertices[0].pos);
		glTexCoordPointer(2, GL_FLOAT,         sizeof(Vertex), &_vertices[0].tex);
		glColorPointer(   4, GL_UNSIGNED_BYTE, sizeof(Vertex), &_vertices[0].col);

		glDrawArrays(GL_LINES, 0, _numVertices);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glDisable(GL_BLEND);

	} // drawLines

	void drawTriangleStrips(const Vertex* _vertices, const unsigned int _numVertices, const Blend::Factor _srcBlendFactor, const Blend::Factor _dstBlendFactor)
	{
		glEnable(GL_BLEND);
		glBlendFunc(convertBlendFactor(_srcBlendFactor), convertBlendFactor(_dstBlendFactor));

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(  2, GL_FLOAT,         sizeof(Vertex), &_vertices[0].pos);
		glTexCoordPointer(2, GL_FLOAT,         sizeof(Vertex), &_vertices[0].tex);
		glColorPointer(   4, GL_UNSIGNED_BYTE, sizeof(Vertex), &_vertices[0].col);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, _numVertices);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glDisable(GL_BLEND);

	} // drawTriangleStrips

	void setProjection(const Transform4x4f& _projection)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((GLfloat*)&_projection);

	} // setProjection

	void setMatrix(const Transform4x4f& _matrix)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((GLfloat*)&_matrix);

	} // setMatrix

	void setViewport(const Rect& _viewport)
	{
		// glViewport starts at the bottom left of the window
		glViewport( _viewport.x, getWindowHeight() - _viewport.y - _viewport.h, _viewport.w, _viewport.h);

	} // setViewport

	void setScissor(const Rect& _scissor)
	{
		if((_scissor.x == 0) && (_scissor.y == 0) && (_scissor.w == 0) && (_scissor.h == 0))
		{
			glDisable(GL_SCISSOR_TEST);
		}
		else
		{
			// glScissor starts at the bottom left of the window
			glScissor(_scissor.x, getWindowHeight() - _scissor.y - _scissor.h, _scissor.w, _scissor.h);
			glEnable(GL_SCISSOR_TEST);
		}

	} // setScissor

	void setSwapInterval()
	{
		// vsync
		if(Settings::getInstance()->getBool("VSync"))
		{
			if(eglSwapInterval(egl_display, 1) != EGL_TRUE)
				LOG(LogWarning) << "Tried to enable vsync, but failed! (" << (int)eglGetError() << ")";
		}
		else
			eglSwapInterval(egl_display, 0);

	} // setSwapInterval

	void swapBuffers()
	{
		wl_display_dispatch_pending (display);
		eglSwapBuffers (egl_display, window->egl_surface);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	} // swapBuffers

	const int& getWindowWidth()   { return windowWidth; }
	const int& getWindowHeight()  { return windowHeight; }
	const int& getScreenWidth()   { return screenWidth; }
	const int& getScreenHeight()  { return screenHeight; }
	const int& getScreenOffsetX() { return screenOffsetX; }
	const int& getScreenOffsetY() { return screenOffsetY; }
	const int& getScreenRotate()  { return screenRotate; }

} // Renderer::

#endif // USE_WAYLANDGLES
