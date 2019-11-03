/*
  SDL Wayland Input code for PSC
  Adapted from SDL2 source by CompCom Sept 2019
  
  Source Files SDL 2.0.4 - events/scancodes_xfree86.h video/wayland/SDL_waylandevents.c
*/

/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#if defined(USE_WAYLANDGLES)

#include <SDL.h>
#include <xkbcommon/xkbcommon.h>
#include <wayland-client.h>

#include <sys/mman.h>
#include <unistd.h>

static const SDL_Scancode xfree86_scancode_table2[] = {
    /*  0 */    SDL_SCANCODE_UNKNOWN,
    /*  1 */    SDL_SCANCODE_ESCAPE,
    /*  2 */    SDL_SCANCODE_1,
    /*  3 */    SDL_SCANCODE_2,
    /*  4 */    SDL_SCANCODE_3,
    /*  5 */    SDL_SCANCODE_4,
    /*  6 */    SDL_SCANCODE_5,
    /*  7 */    SDL_SCANCODE_6,
    /*  8 */    SDL_SCANCODE_7,
    /*  9 */    SDL_SCANCODE_8,
    /*  10 */   SDL_SCANCODE_9,
    /*  11 */   SDL_SCANCODE_0,
    /*  12 */   SDL_SCANCODE_MINUS,
    /*  13 */   SDL_SCANCODE_EQUALS,
    /*  14 */   SDL_SCANCODE_BACKSPACE,
    /*  15 */   SDL_SCANCODE_TAB,
    /*  16 */   SDL_SCANCODE_Q,
    /*  17 */   SDL_SCANCODE_W,
    /*  18 */   SDL_SCANCODE_E,
    /*  19 */   SDL_SCANCODE_R,
    /*  20 */   SDL_SCANCODE_T,
    /*  21 */   SDL_SCANCODE_Y,
    /*  22 */   SDL_SCANCODE_U,
    /*  23 */   SDL_SCANCODE_I,
    /*  24 */   SDL_SCANCODE_O,
    /*  25 */   SDL_SCANCODE_P,
    /*  26 */   SDL_SCANCODE_LEFTBRACKET,
    /*  27 */   SDL_SCANCODE_RIGHTBRACKET,
    /*  28 */   SDL_SCANCODE_RETURN,
    /*  29 */   SDL_SCANCODE_LCTRL,
    /*  30 */   SDL_SCANCODE_A,
    /*  31 */   SDL_SCANCODE_S,
    /*  32 */   SDL_SCANCODE_D,
    /*  33 */   SDL_SCANCODE_F,
    /*  34 */   SDL_SCANCODE_G,
    /*  35 */   SDL_SCANCODE_H,
    /*  36 */   SDL_SCANCODE_J,
    /*  37 */   SDL_SCANCODE_K,
    /*  38 */   SDL_SCANCODE_L,
    /*  39 */   SDL_SCANCODE_SEMICOLON,
    /*  40 */   SDL_SCANCODE_APOSTROPHE,
    /*  41 */   SDL_SCANCODE_GRAVE,
    /*  42 */   SDL_SCANCODE_LSHIFT,
    /*  43 */   SDL_SCANCODE_BACKSLASH,
    /*  44 */   SDL_SCANCODE_Z,
    /*  45 */   SDL_SCANCODE_X,
    /*  46 */   SDL_SCANCODE_C,
    /*  47 */   SDL_SCANCODE_V,
    /*  48 */   SDL_SCANCODE_B,
    /*  49 */   SDL_SCANCODE_N,
    /*  50 */   SDL_SCANCODE_M,
    /*  51 */   SDL_SCANCODE_COMMA,
    /*  52 */   SDL_SCANCODE_PERIOD,
    /*  53 */   SDL_SCANCODE_SLASH,
    /*  54 */   SDL_SCANCODE_RSHIFT,
    /*  55 */   SDL_SCANCODE_KP_MULTIPLY,
    /*  56 */   SDL_SCANCODE_LALT,
    /*  57 */   SDL_SCANCODE_SPACE,
    /*  58 */   SDL_SCANCODE_CAPSLOCK,
    /*  59 */   SDL_SCANCODE_F1,
    /*  60 */   SDL_SCANCODE_F2,
    /*  61 */   SDL_SCANCODE_F3,
    /*  62 */   SDL_SCANCODE_F4,
    /*  63 */   SDL_SCANCODE_F5,
    /*  64 */   SDL_SCANCODE_F6,
    /*  65 */   SDL_SCANCODE_F7,
    /*  66 */   SDL_SCANCODE_F8,
    /*  67 */   SDL_SCANCODE_F9,
    /*  68 */   SDL_SCANCODE_F10,
    /*  69 */   SDL_SCANCODE_NUMLOCKCLEAR,
    /*  70 */   SDL_SCANCODE_SCROLLLOCK,
    /*  71 */   SDL_SCANCODE_KP_7,
    /*  72 */   SDL_SCANCODE_KP_8,
    /*  73 */   SDL_SCANCODE_KP_9,
    /*  74 */   SDL_SCANCODE_KP_MINUS,
    /*  75 */   SDL_SCANCODE_KP_4,
    /*  76 */   SDL_SCANCODE_KP_5,
    /*  77 */   SDL_SCANCODE_KP_6,
    /*  78 */   SDL_SCANCODE_KP_PLUS,
    /*  79 */   SDL_SCANCODE_KP_1,
    /*  80 */   SDL_SCANCODE_KP_2,
    /*  81 */   SDL_SCANCODE_KP_3,
    /*  82 */   SDL_SCANCODE_KP_0,
    /*  83 */   SDL_SCANCODE_KP_PERIOD,
    /*  84 */   SDL_SCANCODE_SYSREQ,    /* ???? */
    /*  85 */   SDL_SCANCODE_MODE,      /* ???? */
    /*  86 */   SDL_SCANCODE_NONUSBACKSLASH,
    /*  87 */   SDL_SCANCODE_F11,
    /*  88 */   SDL_SCANCODE_F12,
    /*  89 */   SDL_SCANCODE_INTERNATIONAL1, /* \_ */
    /*  90 */   SDL_SCANCODE_UNKNOWN,   /* Katakana */
    /*  91 */   SDL_SCANCODE_UNKNOWN,   /* Hiragana */
    /*  92 */   SDL_SCANCODE_INTERNATIONAL4, /* Henkan_Mode */
    /*  93 */   SDL_SCANCODE_INTERNATIONAL2, /* Hiragana_Katakana */
    /*  94 */   SDL_SCANCODE_INTERNATIONAL5, /* Muhenkan */
    /*  95 */   SDL_SCANCODE_UNKNOWN,
    /*  96 */   SDL_SCANCODE_KP_ENTER,
    /*  97 */   SDL_SCANCODE_RCTRL,
    /*  98 */   SDL_SCANCODE_KP_DIVIDE,
    /*  99 */   SDL_SCANCODE_PRINTSCREEN,
    /* 100 */   SDL_SCANCODE_RALT,      /* ISO_Level3_Shift, ALTGR, RALT */
    /* 101 */   SDL_SCANCODE_UNKNOWN,   /* Linefeed */
    /* 102 */   SDL_SCANCODE_HOME,
    /* 103 */   SDL_SCANCODE_UP,
    /* 104 */   SDL_SCANCODE_PAGEUP,
    /* 105 */   SDL_SCANCODE_LEFT,
    /* 106 */   SDL_SCANCODE_RIGHT,
    /* 107 */   SDL_SCANCODE_END,
    /* 108 */   SDL_SCANCODE_DOWN,
    /* 109 */   SDL_SCANCODE_PAGEDOWN,
    /* 110 */   SDL_SCANCODE_INSERT,
    /* 111 */   SDL_SCANCODE_DELETE,
    /* 112 */   SDL_SCANCODE_UNKNOWN,
    /* 113 */   SDL_SCANCODE_MUTE,
    /* 114 */   SDL_SCANCODE_VOLUMEDOWN,
    /* 115 */   SDL_SCANCODE_VOLUMEUP,
    /* 116 */   SDL_SCANCODE_POWER,
    /* 117 */   SDL_SCANCODE_KP_EQUALS,
    /* 118 */   SDL_SCANCODE_UNKNOWN,   /* plusminus */
    /* 119 */   SDL_SCANCODE_PAUSE,
    /* 120 */   SDL_SCANCODE_UNKNOWN,   /* XF86LaunchA */
    /* 121 */   SDL_SCANCODE_UNKNOWN,   /* KP_Decimal */
    /* 122 */   SDL_SCANCODE_UNKNOWN,   /* Hangul */
    /* 123 */   SDL_SCANCODE_UNKNOWN,   /* Hangul_Hanja */
    /* 124 */   SDL_SCANCODE_INTERNATIONAL3, /* Yen */
    /* 125 */   SDL_SCANCODE_LGUI,
    /* 126 */   SDL_SCANCODE_RGUI,
    /* 127 */   SDL_SCANCODE_APPLICATION,
    /* 128 */   SDL_SCANCODE_CANCEL,
    /* 129 */   SDL_SCANCODE_AGAIN,
    /* 130 */   SDL_SCANCODE_UNKNOWN,   /* SunProps */
    /* 131 */   SDL_SCANCODE_UNDO,
    /* 132 */   SDL_SCANCODE_UNKNOWN,   /* SunFront */
    /* 133 */   SDL_SCANCODE_COPY,
    /* 134 */   SDL_SCANCODE_UNKNOWN,   /* SunOpen */
    /* 135 */   SDL_SCANCODE_PASTE,
    /* 136 */   SDL_SCANCODE_FIND,
    /* 137 */   SDL_SCANCODE_CUT,
    /* 138 */   SDL_SCANCODE_HELP,
    /* 139 */   SDL_SCANCODE_UNKNOWN,   /* XF86MenuKB */
    /* 140 */   SDL_SCANCODE_CALCULATOR,
    /* 141 */   SDL_SCANCODE_UNKNOWN,
    /* 142 */   SDL_SCANCODE_SLEEP,
    /* 143 */   SDL_SCANCODE_UNKNOWN,   /* XF86WakeUp */
    /* 144 */   SDL_SCANCODE_UNKNOWN,   /* XF86Explorer */
    /* 145 */   SDL_SCANCODE_UNKNOWN,   /* XF86Send */
    /* 146 */   SDL_SCANCODE_UNKNOWN,
    /* 147 */   SDL_SCANCODE_UNKNOWN,   /* XF86Xfer */
    /* 148 */   SDL_SCANCODE_APP1,      /* XF86Launch1 */
    /* 149 */   SDL_SCANCODE_APP2,      /* XF86Launch2 */
    /* 150 */   SDL_SCANCODE_WWW,
    /* 151 */   SDL_SCANCODE_UNKNOWN,   /* XF86DOS */
    /* 152 */   SDL_SCANCODE_UNKNOWN,   /* XF86ScreenSaver */
    /* 153 */   SDL_SCANCODE_UNKNOWN,
    /* 154 */   SDL_SCANCODE_UNKNOWN,   /* XF86RotateWindows */
    /* 155 */   SDL_SCANCODE_MAIL,
    /* 156 */   SDL_SCANCODE_AC_BOOKMARKS,   /* XF86Favorites */
    /* 157 */   SDL_SCANCODE_COMPUTER,
    /* 158 */   SDL_SCANCODE_AC_BACK,
    /* 159 */   SDL_SCANCODE_AC_FORWARD,
    /* 160 */   SDL_SCANCODE_UNKNOWN,
    /* 161 */   SDL_SCANCODE_EJECT,
    /* 162 */   SDL_SCANCODE_EJECT,
    /* 163 */   SDL_SCANCODE_AUDIONEXT,
    /* 164 */   SDL_SCANCODE_AUDIOPLAY,
    /* 165 */   SDL_SCANCODE_AUDIOPREV,
    /* 166 */   SDL_SCANCODE_AUDIOSTOP,
    /* 167 */   SDL_SCANCODE_UNKNOWN,   /* XF86AudioRecord */
    /* 168 */   SDL_SCANCODE_UNKNOWN,   /* XF86AudioRewind */
    /* 169 */   SDL_SCANCODE_UNKNOWN,   /* XF86Phone */
    /* 170 */   SDL_SCANCODE_UNKNOWN,
    /* 171 */   SDL_SCANCODE_F13,       /* XF86Tools */
    /* 172 */   SDL_SCANCODE_AC_HOME,
    /* 173 */   SDL_SCANCODE_AC_REFRESH,
    /* 174 */   SDL_SCANCODE_UNKNOWN,   /* XF86Close */
    /* 175 */   SDL_SCANCODE_UNKNOWN,
    /* 176 */   SDL_SCANCODE_UNKNOWN,
    /* 177 */   SDL_SCANCODE_UNKNOWN,   /* XF86ScrollUp */
    /* 178 */   SDL_SCANCODE_UNKNOWN,   /* XF86ScrollDown */
    /* 179 */   SDL_SCANCODE_UNKNOWN,   /* parenleft */
    /* 180 */   SDL_SCANCODE_UNKNOWN,   /* parenright */
    /* 181 */   SDL_SCANCODE_UNKNOWN,   /* XF86New */
    /* 182 */   SDL_SCANCODE_AGAIN,
    /* 183 */   SDL_SCANCODE_F13,       /* XF86Tools */
    /* 184 */   SDL_SCANCODE_F14,       /* XF86Launch5 */
    /* 185 */   SDL_SCANCODE_F15,       /* XF86Launch6 */
    /* 186 */   SDL_SCANCODE_F16,       /* XF86Launch7 */
    /* 187 */   SDL_SCANCODE_F17,       /* XF86Launch8 */
    /* 188 */   SDL_SCANCODE_F18,       /* XF86Launch9 */
    /* 189 */   SDL_SCANCODE_F19,       /* null keysym */
    /* 190 */   SDL_SCANCODE_UNKNOWN,
    /* 191 */   SDL_SCANCODE_UNKNOWN,
    /* 192 */   SDL_SCANCODE_UNKNOWN,   /* XF86TouchpadToggle */
    /* 193 */   SDL_SCANCODE_UNKNOWN,
    /* 194 */   SDL_SCANCODE_UNKNOWN,
    /* 195 */   SDL_SCANCODE_MODE,
    /* 196 */   SDL_SCANCODE_UNKNOWN,
    /* 197 */   SDL_SCANCODE_UNKNOWN,
    /* 198 */   SDL_SCANCODE_UNKNOWN,
    /* 199 */   SDL_SCANCODE_UNKNOWN,
    /* 200 */   SDL_SCANCODE_AUDIOPLAY,
    /* 201 */   SDL_SCANCODE_UNKNOWN,   /* XF86AudioPause */
    /* 202 */   SDL_SCANCODE_UNKNOWN,   /* XF86Launch3 */
    /* 203 */   SDL_SCANCODE_UNKNOWN,   /* XF86Launch4 */
    /* 204 */   SDL_SCANCODE_UNKNOWN,   /* XF86LaunchB */
    /* 205 */   SDL_SCANCODE_UNKNOWN,   /* XF86Suspend */
    /* 206 */   SDL_SCANCODE_UNKNOWN,   /* XF86Close */
    /* 207 */   SDL_SCANCODE_AUDIOPLAY,
    /* 208 */   SDL_SCANCODE_AUDIONEXT,
    /* 209 */   SDL_SCANCODE_UNKNOWN,
    /* 210 */   SDL_SCANCODE_PRINTSCREEN,
    /* 211 */   SDL_SCANCODE_UNKNOWN,
    /* 212 */   SDL_SCANCODE_UNKNOWN,   /* XF86WebCam */
    /* 213 */   SDL_SCANCODE_UNKNOWN,
    /* 214 */   SDL_SCANCODE_UNKNOWN,
    /* 215 */   SDL_SCANCODE_MAIL,
    /* 216 */   SDL_SCANCODE_UNKNOWN,
    /* 217 */   SDL_SCANCODE_AC_SEARCH,
    /* 218 */   SDL_SCANCODE_UNKNOWN,
    /* 219 */   SDL_SCANCODE_UNKNOWN,   /* XF86Finance */
    /* 220 */   SDL_SCANCODE_UNKNOWN,
    /* 221 */   SDL_SCANCODE_UNKNOWN,   /* XF86Shop */
    /* 222 */   SDL_SCANCODE_UNKNOWN,
    /* 223 */   SDL_SCANCODE_STOP,
    /* 224 */   SDL_SCANCODE_BRIGHTNESSDOWN,
    /* 225 */   SDL_SCANCODE_BRIGHTNESSUP,
    /* 226 */   SDL_SCANCODE_MEDIASELECT,
    /* 227 */   SDL_SCANCODE_DISPLAYSWITCH,
    /* 228 */   SDL_SCANCODE_KBDILLUMTOGGLE,
    /* 229 */   SDL_SCANCODE_KBDILLUMDOWN,
    /* 230 */   SDL_SCANCODE_KBDILLUMUP,
    /* 231 */   SDL_SCANCODE_UNKNOWN,   /* XF86Send */
    /* 232 */   SDL_SCANCODE_UNKNOWN,   /* XF86Reply */
    /* 233 */   SDL_SCANCODE_UNKNOWN,   /* XF86MailForward */
    /* 234 */   SDL_SCANCODE_UNKNOWN,   /* XF86Save */
    /* 235 */   SDL_SCANCODE_UNKNOWN,   /* XF86Documents */
    /* 236 */   SDL_SCANCODE_UNKNOWN,   /* XF86Battery */
    /* 237 */   SDL_SCANCODE_UNKNOWN,   /* XF86Bluetooth */
    /* 238 */   SDL_SCANCODE_UNKNOWN,   /* XF86WLAN */
};

extern "C" {
    extern int SDL_SendKeyboardKey(Uint8 state, SDL_Scancode scancode);
    extern int SDL_SendKeyboardText(const char *text);
}

static struct wl_keyboard *keyboard = nullptr;
static struct wl_seat *seat = nullptr;
static struct {
    struct xkb_keymap *keymap = nullptr;
    struct xkb_state *state = nullptr;
} xkb;
static struct xkb_context *xkb_context = nullptr;

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                         uint32_t format, int fd, uint32_t size)
{
    if(xkb.keymap || xkb.state)
        return;

    char *map_str;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    map_str = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (map_str == MAP_FAILED) {
        close(fd);
        return;
    }

    xkb.keymap = xkb_keymap_new_from_string(xkb_context,
                                            map_str,
                                            XKB_KEYMAP_FORMAT_TEXT_V1,
                                            (xkb_keymap_compile_flags)0);
    munmap(map_str, size);
    close(fd);

    if (!xkb.keymap) {
        fprintf(stderr, "failed to compile keymap\n");
        return;
    }

    xkb.state = xkb_state_new(xkb.keymap);
    if (!xkb.state) {
        fprintf(stderr, "failed to create XKB state\n");
        xkb_keymap_unref(xkb.keymap);
        xkb.keymap = NULL;
        return;
    }
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys) { }

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) { }

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                    uint32_t serial, uint32_t time, uint32_t key,
                    uint32_t state_w)
{
    enum wl_keyboard_key_state state = (wl_keyboard_key_state)state_w;
    const xkb_keysym_t *syms;
    uint32_t scancode;
    char text[8];
    int size;

    if (key < SDL_arraysize(xfree86_scancode_table2)) {
        scancode = xfree86_scancode_table2[key];

        // TODO when do we get WL_KEYBOARD_KEY_STATE_REPEAT?
        if (scancode != SDL_SCANCODE_UNKNOWN)
            SDL_SendKeyboardKey(state == WL_KEYBOARD_KEY_STATE_PRESSED ?
                                SDL_PRESSED : SDL_RELEASED, (SDL_Scancode)scancode);
    }

    if (!xkb.state)
        return;

    // TODO can this happen?
    if (xkb_state_key_get_syms(xkb.state, key + 8, &syms) != 1)
        return;

    if (state) {
        size = xkb_keysym_to_utf8(syms[0], text, sizeof text);

        if (size > 0) {
            text[size] = 0;
            SDL_SendKeyboardText(text);
        }
    }
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, uint32_t mods_depressed,
                          uint32_t mods_latched, uint32_t mods_locked,
                          uint32_t group)
{
    xkb_state_update_mask(xkb.state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);
}

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         unsigned int caps)
{
    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !keyboard) {
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && keyboard) {
        wl_keyboard_destroy(keyboard);
        keyboard = nullptr;
    }
}

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
};

static void destroy_wayland_input()
{
    if (keyboard)
        wl_keyboard_destroy(keyboard);
    if (xkb.keymap)
        xkb_keymap_unref(xkb.keymap);
    if (xkb.state)
        xkb_state_unref(xkb.state);
    if (xkb_context)
        xkb_context_unref(xkb_context);

    keyboard = nullptr;
    xkb.keymap = nullptr;
    xkb.state = nullptr;
    xkb_context = nullptr;
}

#endif
