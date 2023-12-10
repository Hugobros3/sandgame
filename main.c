#include "sandgame.h"

#include "cunk/graphics.h"
#include "cunk/math.h"
#include "cunk/print.h"
#include "cunk/io.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

static Window* window;
static GfxCtx* ctx;
static GfxShader* shader;
static GfxBuffer* buffer;
static GfxTexture* tex;

GLFWwindow* gfx_get_glfw_handle(Window*);

float geometryData[] = {
    3.0, -1.0,
    -1.0,  -1.0,
    -1.0,  3.0,
};

struct {
    bool wireframe, face_culling, depth_testing;
    bool finish, vsync;
} config = {
    .depth_testing = true,
};

static float frand() {
    int r = rand();
    double rd = (double) r;
    rd /= (double) RAND_MAX;
    return ((float) rd);
}

static void init_tex() {
    GfxTexFormat f = { .base = GFX_TCF_I32, .num_components = 1 };
    tex = gfx_create_texture(ctx, 640, 480, 0, f);
}

static World* world;
static World* world2;

static void key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS)
        return;

    switch (key) {
        case GLFW_KEY_2:
            config.wireframe ^= true;
            break;
        case GLFW_KEY_3:
            config.face_culling ^= true;
            break;
        case GLFW_KEY_4:
            config.depth_testing ^= true;
            break;
        case GLFW_KEY_5:
            config.finish ^= true;
            break;
        case GLFW_KEY_6:
            config.vsync ^= true;
            break;
        default: break;
    }
}

static void render() {
    gfx_cmd_resize_viewport(ctx, window);
    gfx_cmd_clear(ctx);
    GfxState state = {
        .wireframe = config.wireframe,
        .face_culling = config.face_culling,
        .depth_testing = config.depth_testing,
    };
    gfx_cmd_set_draw_state(ctx, state);
    gfx_cmd_use_shader(ctx, shader);

    size_t width, height;
    gfx_get_window_size(window, &width, &height);

    int iw = width, ih = height;
    gfx_cmd_set_shader_extern(ctx, "width", &iw);
    gfx_cmd_set_shader_extern(ctx, "height", &ih);

    gfx_upload_texture(tex, world->voxels);
    gfx_cmd_set_shader_extern(ctx, "the_texture", tex);

    gfx_cmd_set_vertex_input(ctx, "vertexIn", buffer, 2, sizeof(float) * 2, 0);

    gfx_cmd_draw_arrays(ctx, 0, 3);
}

#define SMOOTH_FPS_ACC_FRAMES 32
static double last_frames_times[SMOOTH_FPS_ACC_FRAMES] = { 0 };
static int frame = 0;

int main() {
    world = make_world(640, 480);
    world2 = make_world(640, 480);

    window = gfx_create_window("Hello", 640, 480, &ctx);
    glfwSetKeyCallback(gfx_get_glfw_handle(window), key_callback);

    char* test_vs, *test_fs;
    size_t test_vs_size, test_fs_size;
    read_file("shaders/test.vs", &test_vs_size, &test_vs);
    read_file("shaders/test.fs", &test_fs_size, &test_fs);
    shader = gfx_create_shader(ctx, test_vs, test_fs);

    buffer = gfx_create_buffer(ctx, sizeof(geometryData));
    gfx_copy_to_buffer(buffer, (void*) geometryData, sizeof(geometryData));

    init_tex();

    fflush(stdout);
    fflush(stderr);

    glfwSwapInterval(config.vsync ? 1 : 0);

    while (!glfwWindowShouldClose(gfx_get_glfw_handle(window))) {
        double then = glfwGetTime();

        add_sand(world);
        tick_world(world, world2);
        World* tmp = world2;
        world2 = world;
        world = tmp;
        render();

        glfwSwapBuffers(gfx_get_glfw_handle(window));
        if (config.finish)
            gfx_wait_for_idle();

        double now = glfwGetTime();
        double frametime = 0.0;
        int available_frames = frame > SMOOTH_FPS_ACC_FRAMES ? SMOOTH_FPS_ACC_FRAMES : frame;
        int j = frame % SMOOTH_FPS_ACC_FRAMES;
        for (int i = 0; i < available_frames; i++) {
            double frame_start = last_frames_times[j];
            j = (j + 1) % SMOOTH_FPS_ACC_FRAMES;
            double frame_end;
            if (i + 1 == available_frames)
                frame_end = now;
            else
                frame_end = last_frames_times[j];
            double delta = frame_end - frame_start;
            frametime += delta;
        }

        frametime /= (double) available_frames;

        double fps = 1.0 / frametime;
        int ifps = (int) fps;

        const char* t = format_string("FPS: %d", ifps);
        glfwSetWindowTitle(gfx_get_glfw_handle(window), t);
        free(t);

        last_frames_times[frame % SMOOTH_FPS_ACC_FRAMES] = now;
        frame++;

        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
