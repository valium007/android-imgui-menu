#include "font.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_android.h"
#include "imgui/imgui_impl_opengl3.h"
#include "xhook.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <dlfcn.h>
#include <jni.h>
#include <pthread.h>

typedef EGLBoolean (*eglSwapBuffersFunc)(EGLDisplay display,
                                         EGLSurface surface);

static eglSwapBuffersFunc original_eglSwapBuffers = NULL;
static bool init = false;
static ImVec4 clear_color = ImVec4(0, 0, 0, 0);
static double g_Time = 0;
int dpi_scale = 3;
static bool setup = false;
static bool show_demo_window = false;
int glWidth;
int glHeight;

// custom newframe function, the one provided by imgui doesnt work and im too
// lazy to fix it
void Android_NewFrame() {
  ImGuiIO &io = ImGui::GetIO();

  int32_t window_width = glWidth;
  int32_t window_height = glHeight;

  int display_width = window_width;
  int display_height = window_height;
  io.DisplaySize = ImVec2((float)window_width, (float)window_height);
  if (window_width > 0 && window_height > 0)
    io.DisplayFramebufferScale = ImVec2((float)display_width / window_width,
                                        (float)display_height / window_height);
  struct timespec current_timespec;
  clock_gettime(CLOCK_MONOTONIC, &current_timespec);
  double current_time = (double)(current_timespec.tv_sec) +
                        (current_timespec.tv_nsec / 1000000000.0);
  io.DeltaTime =
      g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
  g_Time = current_time;
}

void DrawMenu() {
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }
  ImGui::SetNextWindowSize(ImVec2(250 * dpi_scale, 100 * dpi_scale), 0);
  ImGui::Begin("Hello, world!");

  ImGui::Text("This is some useful text.");
  ImGui::Checkbox("Demo Window", &show_demo_window);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void DrawImGuiMenu() {
  if (init) {
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    Android_NewFrame();
    ImGui::NewFrame();
    DrawMenu();
    ImGui::Render();
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
}

void SetupImGui() {

  if (!init) {
    auto context = ImGui::CreateContext();
    if (!context) {
      return;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_cfg;
    io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);
    font_cfg.SizePixels = 22.0f;
    io.IniFilename = NULL;
    /*
    io.KeyMap[ImGuiKey_UpArrow] = 19;
    io.KeyMap[ImGuiKey_DownArrow] = 20;
    io.KeyMap[ImGuiKey_LeftArrow] = 21;
    io.KeyMap[ImGuiKey_RightArrow] = 22;
    io.KeyMap[ImGuiKey_Enter] = 66;
    io.KeyMap[ImGuiKey_Backspace] = 67;
    io.KeyMap[ImGuiKey_PageUp] = 92;
    io.KeyMap[ImGuiKey_PageDown] = 93;
    io.KeyMap[ImGuiKey_Escape] = 111;
    io.KeyMap[ImGuiKey_Delete] = 112;
    io.KeyMap[ImGuiKey_Home] = 122;
    io.KeyMap[ImGuiKey_End] = 123;
    io.KeyMap[ImGuiKey_Insert] = 124;
    io.KeyMap[ImGuiKey_UpArrow] = 19;
    io.KeyMap[ImGuiKey_DownArrow] = 20;
    io.KeyMap[ImGuiKey_LeftArrow] = 21;
    io.KeyMap[ImGuiKey_RightArrow] = 22;
    io.KeyMap[ImGuiKey_Enter] = 66;
    io.KeyMap[ImGuiKey_Backspace] = 67;
    io.KeyMap[ImGuiKey_PageUp] = 92;
    io.KeyMap[ImGuiKey_PageDown] = 93;
    io.KeyMap[ImGuiKey_Escape] = 111;
    io.KeyMap[ImGuiKey_Delete] = 112;
    io.KeyMap[ImGuiKey_Home] = 122;
    io.KeyMap[ImGuiKey_End] = 123;
    io.KeyMap[ImGuiKey_Insert] = 124;
    */
    ImGui::StyleColorsDark();
    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium),
                                   16 * dpi_scale, &font_cfg,
                                   io.Fonts->GetGlyphRangesCyrillic());

    init = true;
  }
}

EGLBoolean my_eglSwapBuffers(EGLDisplay display, EGLSurface surface) {
  eglQuerySurface(display, surface, EGL_WIDTH, &glWidth);
  eglQuerySurface(display, surface, EGL_HEIGHT, &glHeight);

  if (!setup) {
    SetupImGui();
  }

  setup = true;
  DrawImGuiMenu();
  EGLBoolean result = original_eglSwapBuffers(display, surface);
  return result;
}

#define HOOK(ret, func, ...)                                                   \
  ret (*orig##func)(__VA_ARGS__);                                              \
  ret my##func(__VA_ARGS__)

// hook for input handling if we dont hook it, then the application doesnt get
// our input
HOOK(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
  origInput(thiz, ex_ab, ex_ac);
  ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
  return;
}

void hook_init() {
  xhook_register(".*\\.so$", "eglSwapBuffers", (void *)my_eglSwapBuffers,
                 (void **)&original_eglSwapBuffers); // hooking eglswapbuffers
  xhook_register(".*\\.so$",
                 "_ZN7android13InputConsumer21initializeMotionEventEPNS_"
                 "11MotionEventEPKNS_12InputMessageE",
                 (void *)myInput, (void **)&origInput); // hooking input handler
  xhook_refresh(1);
}

void *main_thread(void *) {
  hook_init();
  pthread_exit(NULL);
}

__attribute__((constructor)) void _init() {}

extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM *vm, void *key) {
  if (key != (void *)1337)
    return JNI_VERSION_1_6;
  pthread_t t;
  pthread_create(&t, 0, main_thread, 0);
  return JNI_VERSION_1_6;
}
