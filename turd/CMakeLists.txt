project(turd)

include_directories(
    .
    third_party
    ${Boost_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}

    ${CMAKE_SOURCE_DIR}/ext/imgui
    ${CMAKE_SOURCE_DIR}/ext/imgui/examples
    ${CMAKE_SOURCE_DIR}/ext/glTF-SDK/Inc
    ${CMAKE_SOURCE_DIR}/ext/yaml-cpp/include
    ${CMAKE_SOURCE_DIR}/ext/bullet3/src
    ${CMAKE_SOURCE_DIR}/ext/DirectXTex/DirectXTex
    ${CMAKE_CURRENT_BINARY_DIR}/../ext/sol2/single/include
)

set(_SRC
    ${CMAKE_SOURCE_DIR}/ext/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/ext/imgui/examples/imgui_impl_win32.cpp
    ${CMAKE_SOURCE_DIR}/ext/imgui/examples/imgui_impl_dx12.cpp

    core/Application.cpp
    core/Application.hpp
    core/ByteBuffer.cpp
    core/ByteBuffer.hpp
    core/Environment.cpp
    core/Environment.hpp
    core/EventBus.cpp
    core/EventBus.hpp
    core/IO.cpp
    core/IO.hpp
    core/Log.cpp
    core/Log.hpp
    core/Platform.cpp
    core/Platform.hpp
    core/Settings.cpp
    core/Settings.hpp
    core/Timer.cpp
    core/Timer.hpp
    core/Utils.cpp
    core/Utils.hpp
    core/Window.cpp
    core/Window.hpp

    entity/Entity.cpp
    entity/Entity.hpp

    managers/EntityManager.cpp
    managers/EntityManager.hpp
    managers/MaterialManager.cpp
    managers/MaterialManager.hpp
    managers/PipelineManager.cpp
    managers/PipelineManager.hpp

    systems/ISystem.hpp
    systems/input/InputSystem.cpp
    systems/input/InputSystem.hpp
    systems/render/RenderSystem.cpp
    systems/render/RenderSystem.hpp
    systems/render/dx12/Adapter.cpp
    systems/render/dx12/Adapter.hpp
    systems/render/dx12/ConstantBuffer.cpp
    systems/render/dx12/ConstantBuffer.hpp
    systems/render/dx12/DX12State.cpp
    systems/render/dx12/DX12State.hpp
    systems/render/dx12/Frame.cpp
    systems/render/dx12/Frame.hpp
    systems/render/dx12/Pipeline.cpp
    systems/render/dx12/Pipeline.hpp

    main.cpp
)

set(_TP_SRC
    third_party/d3dx12.h
    third_party/sqlite3.c
    third_party/sqlite3.h
    third_party/sqlite3ext.h
)

add_msvc_precompiled_header("pch.hpp" "pch.cpp" _SRC)
add_executable(game WIN32 ${_SRC} ${_TP_SRC})

target_link_libraries(game ${LUA_LIBRARIES} d3d12 dxguid dxgi Bullet3Common Bullet3Collision LinearMath yaml-cpp GLTFSDK)
