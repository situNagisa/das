#pragma once

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <glad/glad.h>

#define NGS_USE_HPP
#include "NGS/basic/basic.h"
#include "NGS/external/glfw.h"

#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include "backend/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "implot.h"

#include "ImFileDialog.h"

//#define LASER_CONFIG_USE_HPP
#include "laser/laser.h"

#define NGS_LIB_GLOBAL_BEGIN
#define NGS_LIB_GLOBAL_END

#define NGS_LIB_NAME das
#define NGS_LIB_VERSION_MAJOR 0
#define NGS_LIB_VERSION_MINOR 1
#define NGS_LIB_VERSION_PATCH 0
#include "NGS/lib/lib.h"