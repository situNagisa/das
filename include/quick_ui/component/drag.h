#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

namespace _detail
{
	NGS_DECLARE_TV(imgui_scalar, ::ImGuiDataType_);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::int8_t, ::ImGuiDataType_S8);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::int16_t, ::ImGuiDataType_S16);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::int32_t, ::ImGuiDataType_S32);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::int64_t, ::ImGuiDataType_S64);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::uint8_t, ::ImGuiDataType_U8);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::uint16_t, ::ImGuiDataType_U16);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::uint32_t, ::ImGuiDataType_U32);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::uint64_t, ::ImGuiDataType_U64);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::float_t, ::ImGuiDataType_Float);
	NGS_DEFINE_TV(imgui_scalar, ::ImGuiDataType_, ::std::double_t, ::ImGuiDataType_Double);
}


template<class T>
	requires ::ngs::cpt::naked_same_as<::ImGuiDataType_, decltype(_detail::imgui_scalar_convert<T>)>
decltype(auto) drag(
	::std::string_view label, 
	T& value, 
	const ::std::type_identity_t<T>& stride = 1,
	const ::std::type_identity_t<T>& min = {},
	const ::std::type_identity_t<T>& max = {},
	::std::string_view format = "",
	::ImGuiSliderFlags_ flags = {}
)
{
	return ::ImGui::DragScalar(label.data(), _detail::imgui_scalar_convert<T>, &value, stride, &min, &max, format.empty() ? nullptr : format.data(), flags);
}

NGS_LIB_MODULE_END