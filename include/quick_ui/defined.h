#pragma once

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "implot.h"

#include "ImFileDialog.h"

#include "NGS/basic/basic.h"
#include "NGS/assert/assert.h"
#include "NGS/external/stl/range.h"
#include "NGS/external/glfw.h"
#include "NGS/symbol/symbol.h"
#include "NGS/to_string/to_string.h"
#include "NGS/enum/enum.h"

#include "NGS/external/opengl_new/texture.h"

#define NGS_LIB_GLOBAL_BEGIN
#define NGS_LIB_GLOBAL_END

#define NGS_LIB_NAME quick_ui
#define NGS_LIB_VERSION_MAJOR 1
#define NGS_LIB_VERSION_MINOR 0
#define NGS_LIB_VERSION_PATCH 0
#include "NGS/lib/lib.h"